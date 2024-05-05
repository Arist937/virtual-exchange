#include "include/exchange.h"

int main() {
    Exchange NYSE(6379);
    NYSE.open();

    return 0;
}