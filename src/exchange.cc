#include <stdio.h>
#include <chrono>

#include "include/exchange.h"

void Exchange::add_order(offer_t offer, string product) {
    using std::chrono::steady_clock;

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