#include <tuple>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>

struct offer_t;

using std::string,
    std::unordered_map,
    std::priority_queue,
    std::tuple,
    std::vector,
    std::chrono::steady_clock;

template<typename T>
using OrderBook = unordered_map<string, priority_queue<offer_t, vector<offer_t>, T>>;

enum class quote_t {
    BID,
    ASK
};

const unordered_map<string, quote_t> str_to_quote_t = {
    {"BID", quote_t::BID},
    {"ASK", quote_t::ASK}
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

struct BidComparator {
    bool operator() (offer_t &a, offer_t &b) {
        if (a.price == b.price) {
            return a.time > b.time;
        }

        return a.price < b.price;
    }
};

struct AskComparator {
    bool operator() (offer_t &a, offer_t &b) {
        if (a.price == b.price) {
            return a.time > b.time;
        }

        return a.price > b.price;
    }
};

class Exchange {
private:
    OrderBook<BidComparator> bid_queues;
    OrderBook<AskComparator> ask_queues;

    void match(string product);
public:
    void add_order(offer_t offer, string product);
};