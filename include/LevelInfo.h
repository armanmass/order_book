#pragma once
#include "Order.h"
#include <vector>

struct LevelInfo {
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos {
    public:
        OrderbookLevelInfos(const LevelInfos& asks, const LevelInfos& bids)
            : asks_{ asks } , bids_{ bids } 
            { }
        
        const LevelInfos& getAsks() const { return asks_; };
        const LevelInfos& getBids() const { return bids_; };

    private:
        LevelInfos bids_;
        LevelInfos asks_;

};