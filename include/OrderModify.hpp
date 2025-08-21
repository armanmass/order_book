#pragma once
#include "Order.hpp"

class OrderModify {
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) :
        orderId_{ orderId }, side_{ side }, price_{ price }, quantity_{ quantity }
        {}
    OrderId getOrderID() const { return orderId_; }
    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }
    Quantity getQuantity() const { return quantity_; }

    OrderPointer toOrderPointer(OrderType orderType){
        return std::make_shared<Order>(orderType, orderId_, side_, price_, quantity_);
    }
private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};