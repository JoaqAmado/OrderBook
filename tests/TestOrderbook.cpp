#include <iostream>
#include <gtest/gtest.h>
#include "Orderbook.h"



TEST(Orderbook, empty_book)
{
    Orderbook book;
    EXPECT_TRUE(book.getOrders().empty());
}

TEST(Orderbook, adding_orders)
{
    Orderbook book;
}
