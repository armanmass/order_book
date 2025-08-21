#pragma once
#include <chrono>
#include <cstdint>
#include <format>
#include <limits>
#include <stdexcept>
#include <memory>
#include <list>
#include <vector>

using OrderId  = std::uint64_t;
using Price    = std::int64_t;
using Quantity = std::uint64_t;
using OrderIds = std::vector<OrderId>;


enum class OrderType { GoodTillCancel, FillAndKill, Market, GoodForDay, FillOrKill };
enum class Side { Buy, Sell };

static const Price InvalidPrice = std::numeric_limits<Price>::quiet_NaN();

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
    
    Order(OrderId orderId, Side side, Quantity quantity)
        : Order(OrderType::Market, orderId, side, InvalidPrice, quantity)
        {}
    
    OrderType getOrderType() const { return orderType_; }
    OrderId getOrderID() const { return orderId_; }

    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }

    Quantity getInitQuantity() const { return initQuantity_; }
    Quantity getRemQuantity() const { return remQuantity_; }
    Quantity getFilledQuantity() const { return initQuantity_ - remQuantity_; }

    void Fill(Quantity quantity)
    {
        if (quantity > remQuantity_)
            throw std::logic_error(std::format("Order ({}) was not filled, not enough quantity remaining.", remQuantity_));
        
        remQuantity_ -= quantity;
    }

    void toGoodTillCancel(Price price)
    {
        if (orderType_ != OrderType::Market)
            throw std::logic_error("Only Market order types can be converted to GoodTillCancel.");
        price_ = price;
        orderType_ = OrderType::GoodTillCancel;
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