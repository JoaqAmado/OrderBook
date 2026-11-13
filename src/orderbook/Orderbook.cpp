#include "Orderbook.h"

uint64_t Order::countId = 0;
//Order
//Constructor
Order::Order(OrderType type_, OrderSide side_, uint64_t initialQuantity_, int64_t price_, Order* nextOrder_, Order* prevOrder_) :
    type(type_), side(side_), price(price_), initialQuantity(initialQuantity_), nextOrder(nextOrder_), prevOrder(prevOrder_)
{
    ID = countId;
    remainingQuantity = initialQuantity;
    ++countId;
}

//Order Getters
uint64_t Order::getID() const {return ID;}
OrderType Order::getType() const {return type;}
OrderSide Order::getSide() const {return side;}
int64_t Order::getPrice() const {return price;}
uint64_t Order::getInitialQuantity() const {return initialQuantity;}
uint64_t Order::getRemainingQuantity() const {return remainingQuantity;}
uint64_t Order::getFillQuantity() const {return Order::getInitialQuantity() - Order::getRemainingQuantity();}
void Order::fill(uint64_t quantity) {remainingQuantity -= quantity;}
Order* Order::getNextOrder() const {return nextOrder;}
Order* Order::getPrevOrder() const {return prevOrder; }

//Orderbook Getters
std::map<int64_t, priceLevel> Orderbook::getBidPriceLevels () const {return bidPriceLevels;}
std::map<int64_t, priceLevel> Orderbook::getAskPriceLevels () const {return askPriceLevels;}
std::unordered_map<uint64_t, Order> Orderbook::getOrders () const {return orders;}


//Setters
void Order::setNextOrder(Order* order) { nextOrder = order; }
void Order::setPrevOrder(Order* order) { prevOrder = order; }

// Orderbook
Orderbook::Orderbook(){}
Orderbook::Orderbook(std::unordered_map<uint64_t, Order> orders_, std::map<int64_t, priceLevel> bidPriceLevels_, std::map<int64_t, priceLevel> askPriceLevels_) :
    orders(orders_), bidPriceLevels(bidPriceLevels_), askPriceLevels(askPriceLevels_)
{}

void Orderbook::add(Order order)
{   
    Orderbook::match(order);

    if (order.getType() == OrderType::Market || order.getRemainingQuantity() == 0)
        return;

    auto [it, inserted] = orders.emplace(order.getID(), std::move(order));
    if (!inserted) return;
    Order* stored = &(it->second);

    auto rest = [stored](auto& levels)
    {
        auto [lvlIt, created] = levels.try_emplace(stored->getPrice());
        priceLevel& level = lvlIt->second;

        if (created)
        {
            level.price = stored->getPrice();
            level.head = stored;
            level.tail = stored;
            level.quantity = stored->getRemainingQuantity();
        }
        else
        {
            level.quantity += stored->getRemainingQuantity();
            if (!level.head)
            {
                level.head = stored;
                level.tail = stored;
            }
            else
            {
                level.tail->setNextOrder(stored);
                stored->setPrevOrder(level.tail);
                level.tail = stored;
            }
        }
    };

    if (stored->getSide() == OrderSide::Buy) {rest(bidPriceLevels);}
    else {rest(askPriceLevels);}
}

void Orderbook::remove(uint64_t orderId)
{
    Order& order = orders.at(orderId);
    auto& priceLevels = order.getSide() == OrderSide::Buy ? bidPriceLevels : askPriceLevels;

    Order* prevOrder = order.getPrevOrder();
    Order* nextOrder = order.getNextOrder();
    if (prevOrder)
    {
        prevOrder->setNextOrder(nextOrder);
    }else
    {
        priceLevels[order.getPrice()].head = nextOrder;
    }
    
    if (nextOrder)
    {
        nextOrder->setPrevOrder(prevOrder);
    }else
    {
        priceLevels[order.getPrice()].tail = prevOrder;
    }
    priceLevels[order.getPrice()].quantity -= order.getRemainingQuantity();
    orders.erase(orderId);
}

void Orderbook::clearPriceLevel(priceLevel& level)
{
    Order* order = level.head;
    while (order)
    {
        orders.erase(order->getID());
        Order* prev = order;
        order = order->getNextOrder();
        delete prev;
    }
}

bool Orderbook::match(Order& order)
{
    OrderType type = order.getType();
    auto& levels = order.getSide() == OrderSide::Buy ? bidPriceLevels : askPriceLevels;
    auto current = levels.begin();

    if (order.getSide() == OrderSide::Buy)
    {
        if (askPriceLevels.empty()) {return false;}

        priceLevel& currentPriceLevel = current->second;
        if (type == OrderType::Market)
        {
            while (!askPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity())
            {
                order.fill(currentPriceLevel.quantity);
                Orderbook::clearPriceLevel(currentPriceLevel);
                current = askPriceLevels.erase(current);
            }
            if (askPriceLevels.empty())
            {
                return true;
            }
            currentPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
        else if (type == OrderType::Limit)
        {
            while (!askPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity() && currentPriceLevel.price <= order.getPrice())
            {
                order.fill(currentPriceLevel.quantity);
                Orderbook::clearPriceLevel(currentPriceLevel);
                current = askPriceLevels.erase(current);
            }
            if (askPriceLevels.empty() || currentPriceLevel.price > order.getPrice())
            {
                return true;
            }
            currentPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
    }
    else if (order.getSide() == OrderSide::Sell)
    {
        if (!bidPriceLevels.empty()) {return false;}

        priceLevel& currentPriceLevel = current->second;
        if (order.getType() == OrderType::Market)
        {
            while (!bidPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity())
            {
                order.fill(currentPriceLevel.quantity);
                Orderbook::clearPriceLevel(currentPriceLevel);
                auto prev = current;
                --current;
                bidPriceLevels.erase(prev);
            }
            if (askPriceLevels.empty())
            {
                return true;
            }
            currentPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
        else if (order.getType() == OrderType::Limit)
        {
            while (!bidPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity() && currentPriceLevel.price >= order.getPrice())
            {
                order.fill(currentPriceLevel.quantity);
                Orderbook::clearPriceLevel(currentPriceLevel);
                auto prev = current;
                --current;
                bidPriceLevels.erase(prev);
            }
            if (askPriceLevels.empty())
            {
                return true;
            }
            currentPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
    }

    if (order.getRemainingQuantity() == 0 && order.getType() != OrderType::Market)
    {
        Orderbook::remove(order.getID());
    }
    return true;
}


