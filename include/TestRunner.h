#include "Orderbook.h"
#include <iostream>
#include <vector>
#include <chrono>

class TestRunner
{
    private:
        //Measures total time
        std::chrono::steady_clock::time_point total_start;
    
        //Measures time per test
        std::chrono::steady_clock::time_point test_start;

    public:
        TestRunner();
        ~TestRunner();
        void start();
        void end();
};
