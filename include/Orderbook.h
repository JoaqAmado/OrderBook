#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <iostream>
#include <chrono>
#include <vector>

struct Order
{
    uint32_t ID;
    char type[8];
    float price;
    uint32_t quantity;
    std::chrono::steady_clock::time_point timestamp;
};

class Orderbook{
    private:
        static int countId;
        std::vector<Order> orders;
    public:
        Orderbook();
        void add(Order order);
        void remove(Order order);
        void match(Order order);
};

#endif
