#include <tuple>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include "types.h"

using std::string, std::priority_queue, std::vector;

template<typename T>
using OrderBook = unordered_map<string, priority_queue<offer_t, vector<offer_t>, T>>;

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
    int port;

    OrderBook<BidComparator> bid_queues;
    OrderBook<AskComparator> ask_queues;

    void match(string product);
    int handle_request(int client_fd);
public:
    Exchange(int port) : port(port) {}

    int open();
    void add_order(offer_t offer, string product);
};