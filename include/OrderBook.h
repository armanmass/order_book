#pragma once
#include "Order.h"

#include <map>
#include <deque>

class OrderBook {
    public:
        static getInstance();
        bool createOrder(Order::ID id, Order::Price price, Order::Quantity quantity);
        bool cancelOrder(Order::ID id);
    private:
        map<Order::Price, std::deque<std::pair<Order::ID, Order::Quantity>>> bids;
        map<Order::Price, std::deque<std::pair<Order::ID, Order::Quantity>>> asks;
}