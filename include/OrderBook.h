#pragma once
#include "Order.h"
#include "TradeInfo.h"

#include <map>
#include <unordered_map>

class OrderBook {
    public:
        bool hasMatch(Side side, Price price) const {
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

        Trades matchOrders(Side side, Price price) {
            Trades trades;
            if (!hasMatch(side, price))
                return t;

            if (side == Side::Buy)
            {
                while (!asks_.empty() && )
            }
            else
            {

            }
            

            return t;
        }

    private:
        struct OrderEntry {
            OrderPointer order_{ nullptr };
            OrderPointers::iterator location_;
        };
        std::map<Price, OrderPointers, std::greater<Price>> bids_;
        std::map<Price, OrderPointers, std::less<Price>> asks_;
        // enable O(1) look up on bids/asks? store pointer to
        // order id to not have to search map
        std::unordered_map<OrderId, OrderEntry> orders_;

};