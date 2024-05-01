#include <iostream>
#include "include/exchange.h"

using std::cout;

void Exchange::add_order(string product, quote_t type, float price, int quantity) {
    switch (type) {
        case quote_t::BID:
            bid_queues[product].emplace(price, 0, quantity, type);
            break;
        case quote_t::ASK:
            ask_queues[product].emplace(price, 0, quantity, type);
            break;
    }

    match(product);
}

void Exchange::match(string product) {
    auto bid_queue = bid_queues[product];
    auto ask_queue = ask_queues[product];

    while (!bid_queue.empty() && !ask_queue.empty()) {
        offer_t best_bid = bid_queue.top();
        bid_queue.pop();
        
        offer_t best_ask = ask_queue.top();
        ask_queue.pop();

        if (best_bid.price < best_ask.price) {
            break;
        }

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

        cout << "Exchanged " << product << ": " << quantity_exchanged << '\n';
    }
}