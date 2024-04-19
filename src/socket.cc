#include <sys/socket.h>

int main() {
    int exchange_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (exchange_fd < 0) {
        return 1;
    }

    return 0;
}