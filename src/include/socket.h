struct Exchange;

enum class RequestType {
    Order,
};

union RequestMessage {
    char buffer[1024];
    struct {
        RequestType type;
        char value[1020];
    };
};

int handle_request(int client_fd, Exchange& exchange);