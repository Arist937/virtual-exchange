#include <tuple>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>

struct offer_t;

using std::string,
    std::unordered_map,
    std::priority_queue,
    std::tuple,
    std::vector;

template<typename T>
using OrderBook = unordered_map<string, priority_queue<offer_t, vector<offer_t>, T>>;

enum class quote_t {
    BID,
    ASK
};

struct offer_t {
    float price;
    int time;
    int quantity;
    quote_t type;
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
    void add_order(string product, quote_t type, float price, int quantity);
};