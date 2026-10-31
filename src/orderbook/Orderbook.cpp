#include "Orderbook.h"

int Orderbook::countId = 0;

Orderbook::Orderbook()
{

}

void Orderbook::add(Order order)
{
    order.timestamp = std::chrono::steady_clock::now();
    order.ID = countId;
    orders.push_back(order);
    Orderbook::match(order);
    ++countId;
}

void Orderbook::remove(Order order)
{
    for (int i {0}; i < orders.size(); ++i)
    {
        if (orders[i].ID == order.ID)
        {
            orders.erase(orders.begin() + i);
        }
    }
}

void Orderbook::match(Order order)
{
    for (int i {0}; i < orders.size(); ++i)
    {
        if (orders[i].ID != order.ID && orders[i].price == order.price)
        {
            orders.erase(orders.begin() + i);
        }
    }
}

