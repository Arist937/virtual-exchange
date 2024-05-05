#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>

#include "include/exchange.h"
#include "include/socket.h"

using std::cerr, std::string;

int main(int argc, char **argv) {
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

  Exchange exchange;

  // Event loop begins
  do {
    std::cout << "Polling...\n";
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
          std::cout << "Accepting incoming clients...\n";
          int client_fd;
          // Loop to accept all incoming connections
          do {
            client_fd = accept(server_fd, NULL, NULL);
            if (client_fd < 0) {
              std::cerr << "Client connection failed\n";
              break;
            }

            fds[nfds].fd = client_fd;
            fds[nfds].events = POLLIN;
            ++nfds;
          } while (client_fd != -1);
        }
        // Case: Client is ready for data
        else {
          std::cout << "Handling client request...\n";
          int bytes = handle_request(fds[i].fd, exchange);
          if (bytes < 0) {
            std::cerr << "Error handling client request\n";
            return 1;
          }

          if (bytes == 0) {
            std::cout << "Client closed\n";
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

int handle_request(int client_fd, Exchange& exchange) {
    // RequestMessage request;
    char buffer[1024];
    int bytes = recv(client_fd, buffer, 1024, 0);
    if (bytes <= 0) {
        return bytes;
    }

    std::string req(buffer, 0, bytes);
    std::stringstream req_stream(req);
    RequestType dummy = RequestType::Order;
    // Format: PRODUCT TYPE PRICE QUANTITY
    switch (dummy) {
        case RequestType::Order:
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
            exchange.add_order(offer, product);
            break;
        }
    }

    return bytes;
}
