#include "Orderbook.h"
#include "TestRunner.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <thread>

int main()
{
    TestRunner testRunner;
    Orderbook testbook;

    testRunner.start();
    std::cout << "Testing add..." << std::endl;
    testbook.add(Order{.price = 12.00f});
    testbook.add(Order{.price = 12.00f});
    testbook.add(Order{.price = 12.00f});
    testbook.add(Order{.price = 12.00f});
    testbook.add(Order{.price = 12.00f});
    testbook.add(Order{.price = 12.00f});
    testRunner.end();

    return 0;
}
