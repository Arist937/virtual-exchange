#include "../src/include/types.h"
#include "../src/include/exchange.h"

int main() {
    Exchange test_exchange(6379);

    test_exchange.add_order("APPL", 0, 1.5, 50, quote_t::ASK);
    test_exchange.add_order("APPL", 0, 1.4, 20, quote_t::ASK);
    test_exchange.add_order("APPL", 1, 1.51, 60, quote_t::BID);
    test_exchange.add_order("APPL", 1, 1.5, 20, quote_t::BID);
    test_exchange.add_order("APPL", 0, 0.7, 20, quote_t::ASK);
    test_exchange.add_order("APPL", 1, 0.6, 100, quote_t::BID);
    test_exchange.add_order("APPL", 0, 0.3, 100, quote_t::ASK);

    return 0;
}