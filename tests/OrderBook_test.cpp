#include "gtest/gtest.h"
#include <memory>
#include <stdexcept>
#include "Order.h"
#include "OrderBook.h"

using namespace ::testing;

class OrderBookTest : public Test
{
protected:
    void SetUp() override
    { }

    void TearDown() override
    { }

    OrderBook book;
};

TEST_F(OrderBookTest, IsEmptyInit)
{
    ASSERT_EQ(book.size(), 0);
    auto infos = book.getOrderInfos();
    ASSERT_TRUE(infos.getAsks().empty());
    ASSERT_TRUE(infos.getBids().empty());
}

TEST_F(OrderBookTest, AddSingleOrder)
{
    auto order = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Buy,
        (Price)100,
        (Quantity)10
    );

    book.addOrder(order);

    ASSERT_EQ(book.size(), 1);
    auto infos = book.getOrderInfos();

    ASSERT_TRUE(infos.getAsks().empty());
    ASSERT_EQ(infos.getBids().size(),1 );

    const auto& tradeInfo = infos.getBids().front();
    ASSERT_EQ(tradeInfo.price_, 100);
    ASSERT_EQ(tradeInfo.quantity_, 10);
}


TEST_F(OrderBookTest, CancelSingleOrder)
{
    auto order = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Buy,
        (Price)100,
        (Quantity)10
    );

    book.addOrder(order);
    ASSERT_EQ(book.size(), 1);

    book.cancelOrder(1);
    ASSERT_EQ(book.size(), 0);

    auto infos = book.getOrderInfos();
    ASSERT_TRUE(infos.getAsks().empty());
    ASSERT_EQ(infos.getBids().size(),1 );

    const auto& tradeInfo = infos.getBids().front();
    ASSERT_EQ(tradeInfo.price_, 100);
    ASSERT_EQ(tradeInfo.quantity_, 10);
}

TEST_F(OrderBookTest, CancelNonExisting)
{
    book.cancelOrder(123);
    ASSERT_EQ(book.size(), 0);
}

TEST_F(OrderBookTest, OrderIdConflict)
{
    auto buyOrder = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Buy,
        (Price)100,
        (Quantity)10
    );

    auto sellOrder= std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Sell,
        (Price)100,
        (Quantity)10
    );

    book.addOrder(buyOrder);
    EXPECT_THROW(book.addOrder(sellOrder), std::logic_error);
}


TEST_F(OrderBookTest, SingleMatch)
{
    auto buyOrder = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Buy,
        (Price)100,
        (Quantity)10
    );

    auto sellOrder= std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)2,
        Side::Sell,
        (Price)100,
        (Quantity)10
    );



}