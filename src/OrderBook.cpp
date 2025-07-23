#include "OrderBook.h"
#include "Order.h"

Trades OrderBook::addOrder(OrderPointer order)
{
    if (orders_.contains(order->getOrderID()))
        return { };

    if (order->getOrderType() == OrderType::FillAndKill && !hasMatch(order->getSide(), order->getPrice()))
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

    return matchOrders();
}

void OrderBook::cancelOrder(OrderId orderId)
{
    if (orders_.contains(orderId))
        return;

    auto& [order, location] = orders_[orderId];

    if (order->getSide() == Side::Buy)
    {
        auto& bidsAtPrice = bids_[order->getPrice()];
        bidsAtPrice.erase(location);
    }
    else
    {
        auto& asksAtPrice  = asks_[order->getPrice()];
        asksAtPrice.erase(location);
    }

    orders_.erase(orderId);
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
