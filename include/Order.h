#pragma once
#include <chrono>
#include <cstdint>


struct Order {
    using ID       = std::uint64_t;
    using Price    = std::uint64_t;
    using Quantity = std::uint64_t;

    ID order_id;
    Price price;
    Quantity quantity;

    enum class Side { Buy, Sell };

    // TODO: remove? insertion order could be use as timestamp
    std::chrono::high_resolution_clock::time_point timestamp;
}