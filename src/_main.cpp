#include "Orderbook.h"
#include "TestRunner.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <thread>

int _main()
{
    TestRunner testRunner;
    Orderbook testbook;

    testRunner.start();
    std::cout << "Testing add..." << std::endl;
    for (int i {0}; i<10000; ++i)
    {
        Order temp(OrderType::Market, OrderSide::Buy, 2000, 100);
        testbook.add(temp);
    }
    std::vector<Order> test = testbook.getOrders();
    for (int i {0}; i < test.size(); ++i)
    {
        std::cout << test[i].getID() << ", " << std::endl; 
    }
    testRunner.end();

    return 0;
}
