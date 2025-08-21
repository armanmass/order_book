#include "gtest/gtest.h"
#include <memory>
#include <stdexcept>
#include "Order.hpp"
#include "OrderBook.hpp"

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
    ASSERT_TRUE(infos.getBids().empty());
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
        (Quantity)2
    );

    book.addOrder(sellOrder);
    Trades trades = book.addOrder(buyOrder);

    ASSERT_EQ(book.size(), 1);
    ASSERT_EQ(trades.size(), 1);

    const auto& trade = trades.front();
    ASSERT_EQ(trade.getBidTrade().orderId_, 1);
    ASSERT_EQ(trade.getAskTrade().orderId_, 2);
    ASSERT_EQ(trade.getBidTrade().price_, 100);
    ASSERT_EQ(trade.getBidTrade().quantity_, 2);

    const auto infos = book.getOrderInfos();
    ASSERT_TRUE(infos.getAsks().empty());
    ASSERT_EQ(infos.getBids().size(), 1);
    ASSERT_EQ(infos.getBids().front().quantity_, 8);
    ASSERT_EQ(infos.getBids().front().price_, 100);
}

TEST_F(OrderBookTest, FillAndKillPartial) {
    auto sellOrder1 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Sell,
        (Price)100,
        (Quantity)5
    );

    auto sellOrder2 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)2,
        Side::Sell,
        (Price)101,
        (Quantity)5
    );

    auto fakOrder=  std::make_shared<Order>(
        OrderType::FillAndKill,
        (OrderId)3,
        Side::Buy,
        (Price)101,
        (Quantity)7
    );

    book.addOrder(sellOrder1);
    book.addOrder(sellOrder2);

    Trades trades = book.addOrder(fakOrder);
    auto infos = book.getOrderInfos();

    ASSERT_EQ(trades.size(), 2);
    ASSERT_EQ(book.size(), 1);
    ASSERT_EQ(trades[0].getBidTrade().quantity_, 5);
    ASSERT_EQ(trades[1].getBidTrade().quantity_, 2);

    ASSERT_EQ(infos.getAsks().front().quantity_, 3);
    ASSERT_EQ(infos.getAsks().front().price_, 101);
}

TEST_F(OrderBookTest, FillOrKill_Fail) {
    auto sellOrder1 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Sell,
        (Price)100,
        (Quantity)5
    );

    auto sellOrder2 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)2,
        Side::Sell,
        (Price)101,
        (Quantity)5
    );

    auto fakOrder=  std::make_shared<Order>(
        OrderType::FillOrKill,
        (OrderId)3,
        Side::Buy,
        (Price)101,
        (Quantity)11
    );

    book.addOrder(sellOrder1);
    book.addOrder(sellOrder2);

    Trades trades = book.addOrder(fakOrder);
    auto infos = book.getOrderInfos();

    ASSERT_EQ(trades.size(), 0);
    ASSERT_EQ(book.size(), 2);

    ASSERT_EQ(infos.getAsks().front().quantity_, 5);
    ASSERT_EQ(infos.getAsks().front().price_, 100);
    ASSERT_EQ(infos.getAsks()[1].quantity_, 5);
    ASSERT_EQ(infos.getAsks()[1].price_, 101);
}


TEST_F(OrderBookTest, FillOrKill_Succeed) {
    auto sellOrder1 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Sell,
        (Price)100,
        (Quantity)5
    );

    auto sellOrder2 = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)2,
        Side::Sell,
        (Price)101,
        (Quantity)5
    );

    auto fakOrder=  std::make_shared<Order>(
        OrderType::FillOrKill,
        (OrderId)3,
        Side::Buy,
        (Price)101,
        (Quantity)10
    );

    book.addOrder(sellOrder1);
    book.addOrder(sellOrder2);

    Trades trades = book.addOrder(fakOrder);
    auto infos = book.getOrderInfos();

    ASSERT_EQ(trades.size(), 2);
    ASSERT_EQ(book.size(), 0);

    ASSERT_TRUE(infos.getAsks().empty());
}