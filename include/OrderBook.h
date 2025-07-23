#pragma once
#include "Order.h"
#include "OrderModify.h"
#include "TradeInfo.h"
#include "LevelInfo.h"

#include <map>
#include <unordered_map>

#include <cstddef>

class OrderBook {
    public:
        Trades addOrder(OrderPointer order);
        void cancelOrder(OrderId orderId);
        Trades modifyOrder(OrderModify modifiedOrder);

        OrderbookLevelInfos getOrderInfos() const;

        std::size_t size() const { return orders_.size(); }

    private:
        bool hasMatch(Side side, Price price) const;

        Trades matchOrders();

        struct OrderEntry {
            OrderPointer order_{ nullptr };
            OrderPointers::iterator location_;
        };

        std::map<Price, OrderPointers, std::greater<Price>> bids_;
        std::map<Price, OrderPointers, std::less<Price>> asks_;

        // enable O(1) look up on bids/asks store pointer to
        // order id to not have to search map O(logn) --> O(1)
        // cost of extra space
        std::unordered_map<OrderId, OrderEntry> orders_;

};