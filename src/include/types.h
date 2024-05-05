#include <chrono>
#include <string>
#include <unordered_map>

using std::string, std::chrono::steady_clock, std::unordered_map;

enum class quote_t {
    BID,
    ASK
};

const unordered_map<string, quote_t> str_to_quote_t = {
    {"BID", quote_t::BID},
    {"ASK", quote_t::ASK}
};

enum class request_t {
    Order,
};

struct offer_t {
    int id;
    float price;
    steady_clock::time_point time;
    int quantity;
    quote_t type;

    offer_t(float price, int quantity, string type) : 
        id(0), price(price), time(steady_clock::now()), quantity(quantity), type(str_to_quote_t.at(type)) {}
};
