#include "OrderBook.h"
#include "Order.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <time.h>

OrderBook::OrderBook()
    : ordersPruneThread_( [this] { pruneGoodForDayOrders(); })
    { }

OrderBook::~OrderBook()
{
    shutdown_.store(true, std::memory_order_release);
    shutdownConditionVariable_.notify_one();
    ordersPruneThread_.join();
}

Trades OrderBook::addOrder(OrderPointer order)
{
    std::scoped_lock ordersLock{ ordersMutex_ };

    if (orders_.contains(order->getOrderID()))
        throw std::logic_error("Duplicate Order IDs.");

    if (order->getOrderType() == OrderType::Market)
    {
        if (order->getSide() == Side::Buy && !asks_.empty())
        {
            const auto& [highestSellPrice, _] = *asks_.rbegin();
            order->toGoodTillCancel(highestSellPrice);
        }
        else if (order->getSide() == Side::Sell && !bids_.empty())
        {
            const auto& [lowestBuyPrice, _] = *bids_.rbegin();
            order->toGoodTillCancel(lowestBuyPrice);
        }
        else
        {
            return { };
        }
    }

    if (order->getOrderType() == OrderType::FillAndKill && !hasMatch(order->getSide(), order->getPrice()))
        return { };

    if (order->getOrderType() == OrderType::FillOrKill && !canFullyFill(order->getSide(), order->getPrice(), order->getRemQuantity()))
        return { };

    OrderPointers::iterator it;

    if (order->getSide() == Side::Buy)
    {
        auto& bidsAtPrice = bids_[order->getPrice()];
        bidsAtPrice.push_back(order);
        it = std::next(bidsAtPrice.begin(), bidsAtPrice.size() - 1);
    }
    else
    {
        auto& asksAtPrice = asks_[order->getPrice()];
        asksAtPrice.push_back(order);
        it = std::next(asksAtPrice.begin(), asksAtPrice.size() - 1);
    }

    orders_[order->getOrderID()] = OrderEntry { order, it };

    onOrderAdded(order);
    return matchOrders();
}


void OrderBook::cancelOrder(OrderId orderId)
{
    std::scoped_lock ordersLock{ ordersMutex_ };

    cancelOrderInternal(orderId);
}


Trades OrderBook::modifyOrder(OrderModify modifiedOrder)
{
    if (!orders_.contains(modifiedOrder.getOrderID()))
        return { };
    
    auto& [order, location] = orders_[modifiedOrder.getOrderID()];
    auto ot = order->getOrderType();

    cancelOrder(modifiedOrder.getOrderID());    
    return addOrder(modifiedOrder.toOrderPointer(ot));
}

OrderbookLevelInfos OrderBook::getOrderInfos() const
{
    LevelInfos asks;
    LevelInfos bids;

    // lambda function will iterate over all prices in asks_ and bids_
    // accumalte each list at each price level in each
    // push back each level to the current level info vector
    auto createLI = [](Price price, const OrderPointers& orders)
    {
        return LevelInfo{ price, std::accumulate(orders.begin(), orders.end(), Quantity{},
                        [](Quantity totalQuantity, const OrderPointer& order){
                            return totalQuantity + order->getRemQuantity();
                        })};
    };

    for (const auto& [price, ordersAtPrice] : asks_)
        asks.push_back(createLI(price, ordersAtPrice));

    for (const auto& [price, ordersAtPrice] : bids_)
        bids.push_back(createLI(price, ordersAtPrice));

    return OrderbookLevelInfos{asks, bids};
}

void OrderBook::pruneGoodForDayOrders()
{
    using namespace std::chrono;
    const auto MarketClose = hours(13);

    while (true)
    {
        const auto now = system_clock::to_time_t(system_clock::now());
        std::tm time_info;
        localtime_r(&now, &time_info);

        if (time_info.tm_hour >= MarketClose.count())
            ++time_info.tm_mday;

        time_info.tm_hour = MarketClose.count();
        time_info.tm_min = 0;
        time_info.tm_sec = 0;

        auto next = system_clock::from_time_t(mktime(&time_info));
        auto time_rem = next - system_clock::from_time_t(now) + seconds(1);

        {
            std::unique_lock ordersLock{ ordersMutex_ };
            
            if (shutdown_.load(std::memory_order_acquire) ||
                shutdownConditionVariable_.wait_for(ordersLock, time_rem) == std::cv_status::no_timeout)
                    return;
        }

        OrderIds orderIds;


        {
            std::scoped_lock ordersLock{ ordersMutex_ };

            for (const auto& [_, entry] : orders_)
            {
                auto& [order, __] = entry;

                if (order->getOrderType() == OrderType::GoodForDay)
                    orderIds.push_back(order->getOrderID());
            }
        }

        cancelOrders(orderIds);
    }
}

void OrderBook::cancelOrders(OrderIds orderIds)
{
    std::scoped_lock ordersLock{ ordersMutex_ };

    for (const auto& orderId : orderIds)
        cancelOrderInternal(orderId);
}

void OrderBook::cancelOrderInternal(OrderId orderId)
{
    if (!orders_.contains(orderId))
        return;

    auto& [order, location] = orders_.at(orderId);

    if (order->getSide() == Side::Buy)
    {
        auto& bidsAtPrice = bids_[order->getPrice()];
        bidsAtPrice.erase(location);
        if (bidsAtPrice.empty())
            bids_.erase(order->getPrice());
    }
    else
    {
        auto& asksAtPrice  = asks_[order->getPrice()];
        asksAtPrice.erase(location);
        if (asksAtPrice.empty())
            asks_.erase(order->getPrice());
    }

    orders_.erase(orderId);
    onOrderCancelled(order);
}

void OrderBook::onOrderAdded(OrderPointer order)
{
    updateLevelData(order->getPrice(), order->getRemQuantity(), LevelData::Action::Add);
}

void OrderBook::onOrderCancelled(OrderPointer order)
{
    updateLevelData(order->getPrice(), order->getRemQuantity(), LevelData::Action::Remove);
}
void OrderBook::onOrderMatched(Price price, Quantity quantity, bool isFullyFilled)
{
    updateLevelData(price, quantity, isFullyFilled ? LevelData::Action::Remove : LevelData::Action::Match);
}

void OrderBook::updateLevelData(Price price, Quantity quantity, LevelData::Action action)
{

    const int change = action == LevelData::Action::Add ? 1 : action == LevelData::Action::Remove ? -1 : 0;
    auto& data = data_[price];

    data.count_ += change;

    if (data.count_ == 0)
    {
        data_.erase(price);
        return;
    }
    
    if (action == LevelData::Action::Add)
    {
        data.quantity_ += quantity;
    }
    else
    {
        data.quantity_ -= quantity;
    }
}


bool OrderBook::hasMatch(Side side, Price price) const {
    if (side == Side::Buy)
    {
        if(!asks_.empty() && asks_.begin()->first <= price)
            return true;
    }
    else
    {
        if(!bids_.empty() && bids_.begin()->first <= price)
            return true;
    }
    return false;
}

bool OrderBook::canFullyFill(Side side, Price price, Quantity quantity) const
{
    std::scoped_lock ordersLock{ ordersMutex_ };

    auto price_iterator = side == Side::Buy ? asks_.begin() : bids_.begin();
    auto end            = side == Side::Buy ? asks_.end()   : bids_.end();

    while (price_iterator != end)
    {
        const auto& [it_price, _] = *price_iterator;

        if ( (side == Side::Buy && it_price > price) ||
             (side == Side::Sell && it_price < price) )
                break;

        const auto& levelData = data_.at(it_price);

        if (quantity <= levelData.quantity_)
            return true;
        quantity -= levelData.quantity_;

        ++price_iterator;
    }

    return false;
}


Trades OrderBook::matchOrders() {
    Trades trades;

    while (true)
    {
        if (asks_.empty() || bids_.empty())
            return trades;

        // get lists based off price priority
        auto& [askPrice, lowestAsks] = *asks_.begin();
        auto& [bidPrice, highestBids] = *bids_.begin();

        if (askPrice > bidPrice)
            return trades;


        // parse lists base off temporal priority
        while (!lowestAsks.empty() && !highestBids.empty())
        {
            auto& ask = lowestAsks.front();
            auto& bid = highestBids.front();

            Quantity quantity = std::min(ask->getRemQuantity(),bid->getRemQuantity());

            ask->Fill(quantity);
            bid->Fill(quantity);

            if (ask->getRemQuantity() == 0)
            {
                lowestAsks.pop_front();
                orders_.erase(ask->getOrderID());
            }

            if (bid->getRemQuantity() == 0)
            {
                highestBids.pop_front();
                orders_.erase(bid->getOrderID());
            }

            if (lowestAsks.empty())
                asks_.erase(askPrice);

            if (highestBids.empty())
                bids_.erase(bidPrice);

            trades.emplace_back(
                TradeInfo { bid->getOrderID(), quantity, bid->getPrice()},
                TradeInfo { ask->getOrderID(), quantity, ask->getPrice()}
            );

            onOrderMatched(bid->getPrice(), quantity, bid->getRemQuantity() == 0);
            onOrderMatched(ask->getPrice(), quantity, ask->getRemQuantity() == 0);
        }

        if (!asks_.empty())
        {
            auto& [_, asks] = *asks_.begin();
            auto& order = asks.front();
            if (order->getOrderType() == OrderType::FillAndKill)
                cancelOrder(order->getOrderID());
        }

        if (!bids_.empty())
        {
            auto& [_, bids] = *bids_.begin();
            auto& order = bids.front();
            if (order->getOrderType() == OrderType::FillAndKill)
                cancelOrder(order->getOrderID());
        }

    }
    return trades;
}