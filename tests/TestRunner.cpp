#include "Orderbook.h"
#include "TestRunner.h"
#include <iostream>
#include <vector>
#include <chrono>

TestRunner::TestRunner()
{
    total_start = std::chrono::steady_clock::now();
    std::cout << "Testing starts now!\n" << std::endl;
}

TestRunner::~TestRunner()
{
    std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start);
    std::cout << "\nTotal Runtime: " << duration.count() << std::endl;
}

void TestRunner::start()
{
    test_start = std::chrono::steady_clock::now();
}

void TestRunner::end()
{
    std::chrono::steady_clock::time_point test_end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(test_end - test_start);
    std::cout << "Test Runtime: " << duration.count() << std::endl;
}
