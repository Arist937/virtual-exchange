#include <stdio.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <poll.h>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "include/exchange.h"

void Exchange::add_order(string product, offer_t offer) {
    switch (offer.type) {
        case quote_t::BID:
            bid_queues[product].push(offer);
            break;
        case quote_t::ASK:
            ask_queues[product].push(offer);
            break;
    }

    match(product);
}

void Exchange::add_order(string product, int id, float price, int quantity, quote_t type) {
    using std::chrono::steady_clock;

    switch (type) {
        case quote_t::BID:
            bid_queues[product].emplace(id, price, steady_clock::now(), quantity, type);
            break;
        case quote_t::ASK:
            ask_queues[product].emplace(id, price, steady_clock::now(), quantity, type);
            break;
    }

    match(product);
}

void Exchange::match(string product) {
    auto& bid_queue = bid_queues[product];
    auto& ask_queue = ask_queues[product];

    while (!bid_queue.empty() && !ask_queue.empty()) {
        offer_t best_bid = bid_queue.top();
        offer_t best_ask = ask_queue.top();

        if (best_bid.price < best_ask.price) {
            break;
        }
        
        bid_queue.pop();
        ask_queue.pop();

        int quantity_exchanged = 0;
        if (best_bid.quantity == best_ask.quantity) {
            quantity_exchanged = best_ask.quantity;
        } else if (best_bid.quantity > best_ask.quantity) {
            best_bid.quantity -= best_ask.quantity;
            quantity_exchanged = best_ask.quantity;

            bid_queue.push(best_bid);
        } else {
            best_ask.quantity -= best_bid.quantity;
            quantity_exchanged = best_bid.quantity;

            ask_queue.push(best_ask);
        }

        printf("%d %s exchanged[Ask ID: %d, Bid ID: %d]\n", quantity_exchanged, product.c_str(), best_ask.id, best_bid.id);
    }
}

int Exchange::open() {
    using std::cerr;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Failed to create server socket\n";
        return 1;
    }

    // Since the tester restarts your program quite often, setting REUSE_PORT
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        cerr << "setsockopt failed\n";
        return 1;
    }

    // Set socket to be non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) {
        cerr << "Error getting flags\n";
        return 1;
    }

    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        cerr << "Error setting non-blocking flag\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        cerr << "Failed to bind to port 6379\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        cerr << "listen failed\n";
        return 1;
    }

    // Set up event loop vars
    struct pollfd fds[100];
    memset(fds, 0, sizeof(fds));

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    nfds_t nfds = 1;
    int timeout = (3 * 60 * 1000);

    // Event loop begins
    do {
        printf("Polling...\n");
        int rc = poll(fds, nfds, timeout);
        if (rc < 0) {
            cerr << "Poll failed\n";
            break;
        }

        if (rc == 0) {
            cerr << "Poll timed out\n";
            break;
        }

        nfds_t current_nfds = nfds;
        for (nfds_t i = 0; i < current_nfds; ++i) {
            if (fds[i].revents & POLLIN) {
                // Case: Incoming connection
                if (fds[i].fd == server_fd) {
                    printf("Accepting incoming clients...\n");
                    int client_fd;
                    // Loop to accept all incoming connections
                    do {
                        client_fd = accept(server_fd, NULL, NULL);
                        printf("Client connection successful [FD: %d]", client_fd);

                        fds[nfds].fd = client_fd;
                        fds[nfds].events = POLLIN;
                        ++nfds;
                    } while (client_fd >= 0);
                }
                // Case: Client is ready for data
                else {
                    printf("Handling client request...\n");
                    int bytes = handle_request(fds[i].fd);
                    if (bytes < 0) {
                        printf("Error handling client request\n");
                        return 1;
                    }

                    if (bytes == 0) {
                        printf("Client closed\n");
                        memset(fds + i, 0, sizeof(pollfd));
                        close(fds[i].fd);
                    }
                }
            }
        }
    } while (true);

    for (nfds_t i = 0; i < nfds; ++i) {
        if(fds[i].fd >= 0) {
            close(fds[i].fd);
        }
    }

    return 0;
}

int Exchange::handle_request(int client_fd) {
    using std::string, std::stringstream;

    // RequestMessage request;
    char buffer[1024];
    int bytes = recv(client_fd, buffer, 1024, 0);
    if (bytes <= 0) {
        return bytes;
    }

    string req(buffer, 0, bytes);
    stringstream req_stream(req);
    request_t dummy = request_t::Order;

    // Format: PRODUCT TYPE PRICE QUANTITY
    switch (dummy) {
        case request_t::Order:
        {
            string product;
            req_stream >> product;

            string type;
            req_stream >> type;

            float price;
            req_stream >> price;

            int quantity;
            req_stream >> quantity;

            printf("ADDING ORDER: %s, %s, %.2f, %d\n", product.c_str(), type.c_str(), price, quantity);
            offer_t offer(price, quantity, type);
            add_order(product, offer);
            break;
        }
    }

    return bytes;
}
