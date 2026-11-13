cmake_minimum_required(VERSION 3.24)

project(Orderbook VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)   # don't build its own tests
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest benchmark)

add_library(OrderbookLib src/orderbook/orderbook.cpp)
target_include_directories(OrderbookLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()
add_executable(OrderbookTests tests/TestOrderbook.cpp)
target_link_libraries(OrderbookTests PRIVATE OrderbookLib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(OrderbookTests)

add_executable(OrderbookBenchmark bench/BenchOrderbook.cpp)
target_link_libraries(OrderbookBenchmark PRIVATE OrderbookLib benchmark::benchmark_main)
