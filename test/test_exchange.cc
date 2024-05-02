#include "../src/include/exchange.h"

int main() {
    Exchange test_exchange;

    test_exchange.add_order(0, "APPL", quote_t::ASK, 1.5, 50);
    test_exchange.add_order(0, "APPL", quote_t::ASK, 1.4, 20);
    test_exchange.add_order(1, "APPL", quote_t::BID, 1.51, 60);
    test_exchange.add_order(1, "APPL", quote_t::BID, 1.5, 20);
    test_exchange.add_order(0, "APPL", quote_t::ASK, 0.7, 20);
    test_exchange.add_order(1, "APPL", quote_t::BID, 0.6, 100);
    test_exchange.add_order(0, "APPL", quote_t::ASK, 0.3, 100);

    return 0;
}