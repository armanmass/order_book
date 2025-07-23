#pragma once
#include "Order.h"
#include "OrderModify.h"
#include "TradeInfo.h"
#include "LevelInfo.h"

#include <mutex>
#include <atomic>
#include <condition_variable>

#include <map>
#include <unordered_map>

#include <cstddef>

class OrderBook {
    public:
        OrderBook();
        ~OrderBook();

        OrderBook(const OrderBook&)       = delete;
        void operator=(const OrderBook&)  = delete;
        OrderBook(const OrderBook&&)      = delete;
        void operator=(const OrderBook&&) = delete;

        Trades addOrder(OrderPointer order);
        void cancelOrder(OrderId orderId);
        Trades modifyOrder(OrderModify modifiedOrder);

        OrderbookLevelInfos getOrderInfos() const;

        std::size_t size() const { return orders_.size(); }

    private:
        struct OrderEntry 
        {
            OrderPointer order_{ nullptr };
            OrderPointers::iterator location_;
        };

        struct LevelData 
        {
            Quantity quantity_{ };
            Quantity count_{ };

            enum class Action { Add, Remove, Match };
        };

        bool hasMatch(Side side, Price price) const;
        Trades matchOrders();

        void pruneGoodForDayOrders();

        void cancelOrders(OrderIds orderIds);
        void cancelOrderInternal(OrderId orderId);

        void onOrderAdded(OrderPointer order);
        void onOrderCancelled(OrderPointer order);
        void onOrderMatched(Price price, Quantity quantity, bool isFullyFilled);
        void updateLevelData(Price price, Quantity quantity, LevelData::Action action);


        std::unordered_map<Price, LevelData> data_;

        std::map<Price, OrderPointers, std::greater<Price>> bids_;
        std::map<Price, OrderPointers, std::less<Price>> asks_;

        std::unordered_map<OrderId, OrderEntry> orders_;

        mutable std::mutex ordersMutex_;
        std::thread ordersPruneThread_;
        
        std::condition_variable shutdownConditionVariable_;
        std::atomic<bool> shutdown_{ false };
};