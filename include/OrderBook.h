#pragma once
#include "Order.h"
#include "TradeInfo.h"

#include <map>
#include <unordered_map>

class OrderBook {
    public:
        void cancelOrder(OrderId orderId);
        void addOrder(OrderId orderId, Quantity quantity, Price price);
        void modifyOrder(OrderId orderId, Quantity quantity, Price price);

    private:
        bool hasMatch(Side side, Price price) const;

        Trades matchOrders(Side side, Price price);

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