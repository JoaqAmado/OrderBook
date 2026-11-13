#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <iostream>
#include <chrono>
#include <vector>
#include <map>

enum class OrderSide
{
    Buy,
    Sell
};

enum class OrderType
{
    Market,
    Limit,
    Stop
};

class Order
{
private:
    static uint64_t countId;
    uint64_t ID;
    OrderType type;
    OrderSide side;
    int64_t price;
    uint64_t initialQuantity;
    uint64_t remainingQuantity;
    Order* nextOrder;
    Order* prevOrder;

public:
    Order(OrderType type_, OrderSide side_, uint64_t initialQuantity_, int64_t price_ = 0, Order* nextOrder_ = nullptr, Order* prevOrder_ = nullptr);

    //Getters
    uint64_t getID () const;
    OrderType getType () const;
    OrderSide getSide () const;
    int64_t getPrice () const;
    uint64_t getInitialQuantity () const;
    uint64_t getRemainingQuantity () const;
    uint64_t getFillQuantity () const;
    void fill(uint64_t quantity);
    Order* getNextOrder () const;
    Order* getPrevOrder () const;

    //Setters
    void setNextOrder(Order* order);
    void setPrevOrder(Order* order);
};

struct priceLevel
{
    int64_t price;
    Order* head;
    Order* tail;
    uint64_t quantity;
};

class Orderbook{
private:
    std::unordered_map<uint64_t, Order> orders;
    std::map<int64_t, priceLevel> bidPriceLevels;
    std::map<int64_t, priceLevel> askPriceLevels;
    
public:
    Orderbook();
    void add(Order order);
    void remove(uint64_t orderId);
    void clearPriceLevel(priceLevel& level);
    bool match(Order& order);

    //Getters
    std::map<int64_t, priceLevel> getBidPriceLevels () const;
    std::map<int64_t, priceLevel> getAskPriceLevels () const;
    std::unordered_map<uint64_t, Order> getOrders () const;
};

#endif
