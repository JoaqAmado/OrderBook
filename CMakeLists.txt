cmake_minimum_required(VERSION 4.4.3)

project(Orderbook VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(OrderbookTests ../src/main.cpp ../src/orderbook/orderbook.cpp ../tests/TestRunner.cpp)

target_include_directories(OrderbookTests PRIVATE include)
