#include "exchange.h"

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

    offer_t best_bid = bid_queue.top();
    bid_queue.pop();
    offer_t best_ask = ask_queue.top();
    ask_queue.pop();
    while (best_bid.price >= best_ask.price) {
        int quantity_exchanged = 0;
        if (best_bid.quantity >= best_ask.quantity) {
            best_bid.quantity -= best_ask.quantity;

            quantity_exchanged = best_ask.quantity;
            best_ask = ask_queue.top();
            ask_queue.pop();
        }

        if (best_ask.quantity >= best_bid.quantity) {
            best_ask.quantity -= best_bid.quantity;

            quantity_exchanged = best_bid.quantity;
            best_bid = bid_queue.top();
            bid_queue.pop();
        }
    }

    if (best_bid.quantity > 0) {
        bid_queue.push(best_bid);
    }

    if (best_ask.quantity > 0) {
        ask_queue.push(best_ask);
    }
}