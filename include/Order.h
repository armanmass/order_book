#pragma once
#include <chrono>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <memory>
#include <list>

using OrderId  = std::uint64_t;
using Price    = std::int64_t;
using Quantity = std::uint64_t;


enum class OrderType { GoodTillCancel, FillAndKill };
enum class Side { Buy, Sell };

class Order {
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity) 
        : orderType_{ orderType }, 
          orderId_{ orderId }, 
          side_{ side }, 
          price_{ price }, 
          initQuantity_{ quantity },
          remQuantity_{ quantity }
        {}
    
    OrderType getOrderType() const { return orderType_; }
    OrderId getOrderID() const { return orderId_; }
    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }
    Quantity getInitQuantity() const { return initQuantity_; }
    Quantity getRemQuantity() const { return remQuantity_; }
    Quantity getFilledQuantity() const { return initQuantity_ - remQuantity_; }

    void Fill(Quantity quantity) {
        if (quantity > remQuantity_)
            throw std::logic_error(std::format("Order ({}) was not filled, not enough quantity remaining.", remQuantity_));
        
        remQuantity_ -= quantity;
    }

private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initQuantity_;
    Quantity remQuantity_;

};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify {
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) :
        orderId_{ orderId }, side_{ side }, price_{ price }, quantity_{ quantity }
        {}
private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;

};