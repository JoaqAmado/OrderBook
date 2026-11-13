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

void Orderbook::add(Order order)
{   
    int64_t price = order.getPrice();
    if (order.getSide() == OrderSide::Buy)
    {
        if (bidPriceLevels.find(price) != bidPriceLevels.end())
        {
            Order* current = bidPriceLevels[price].tail;
            bidPriceLevels[price].tail = &order;
            order.setPrevOrder(current);
            bidPriceLevels[price].quantity += order.getRemainingQuantity();
        }else
        {
            bidPriceLevels[price] = priceLevel {
                .price = price,
                .head = &order,
                .tail = &order,
                .quantity = order.getRemainingQuantity()
            };
        }
    }else if (order.getSide() == OrderSide::Sell)
    {
        if (askPriceLevels.find(price) != askPriceLevels.end())
        {
            Order* current = askPriceLevels[price].tail;
            askPriceLevels[price].tail = &order;
            order.setPrevOrder(current);
            askPriceLevels[price].quantity += order.getRemainingQuantity();
        }else
        {
            askPriceLevels[price] = priceLevel {
                .price = price,
                .head = &order,
                .tail = &order,
                .quantity = order.getRemainingQuantity()
            };
        }
    }
    if (order.getType() != OrderType::Market ) orders[order.getID()] = order;
    Orderbook::match(order);
}

void Orderbook::remove(uint64_t orderId)
{
        auto& priceLevels = orders[orderId].getSide() == OrderSide::Buy ? bidPriceLevels : askPriceLevels;

        Order* prevOrder = orders[orderId].getPrevOrder();
        Order* nextOrder = orders[orderId].getNextOrder();
        if (prevOrder)
        {
            prevOrder->setNextOrder(nextOrder);
        }else
        {
            priceLevels[orders[orderId].getPrice()].head = nextOrder;
        }
        
        if (nextOrder)
        {
            nextOrder->setPrevOrder(prevOrder);
        }else
        {
            priceLevels[orders[orderId].getPrice()].tail = prevOrder;
        }
        priceLevels[orders[orderId].getPrice()].quantity -= orders[orderId].getRemainingQuantity();
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
    if (order.getSide() == OrderSide::Buy)
    {
        if (askPriceLevels.empty())
        {
            return false;
        }
        auto current = askPriceLevels.begin();
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
            priceLevel& bidPriceLevel = bidPriceLevels[order.getPrice()];
            while (!askPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity() && currentPriceLevel.price <= order.getPrice())
            {
                order.fill(currentPriceLevel.quantity);
                bidPriceLevel.quantity -= currentPriceLevel.quantity;
                Orderbook::clearPriceLevel(currentPriceLevel);
                current = askPriceLevels.erase(current);
            }
            if (askPriceLevels.empty() || currentPriceLevel.price > order.getPrice())
            {
                return true;
            }
            currentPriceLevel.quantity -= order.getRemainingQuantity();
            bidPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
    }
    else if (order.getSide() == OrderSide::Sell)
    {
        if (!bidPriceLevels.empty()) {return false;}
        auto current = bidPriceLevels.end();
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
            priceLevel& askPriceLevel = askPriceLevels[order.getPrice()];
            while (!bidPriceLevels.empty() && currentPriceLevel.quantity <= order.getRemainingQuantity() && currentPriceLevel.price >= order.getPrice())
            {
                order.fill(currentPriceLevel.quantity);
                askPriceLevel.quantity -= currentPriceLevel.quantity;
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
            askPriceLevel.quantity -= order.getRemainingQuantity();
            order.fill(order.getRemainingQuantity());
        }
    }

    if (order.getRemainingQuantity() == 0 && order.getType() != OrderType::Market)
    {
        Orderbook::remove(order.getID());
    }
    return true;
}


