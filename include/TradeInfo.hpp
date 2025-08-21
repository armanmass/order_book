#pragma once
#include "Order.hpp"
#include <vector>

struct TradeInfo {
    OrderId orderId_;
    Quantity quantity_;
    Price price_;
};

class Trade {
public:
    Trade(const TradeInfo& bid, const TradeInfo& ask)
        : bid_{ bid }, ask_{ ask }
        {}
    
    const TradeInfo& getBidTrade() const { return bid_; }
    const TradeInfo& getAskTrade() const { return ask_; }
    
private:
    TradeInfo bid_;
    TradeInfo ask_;
};

using Trades = std::vector<Trade>;