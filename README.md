# Order Book

A high-performance, multi-threaded, C++23 implementation financial limit order book.

## Building and Running

### Step 1: Clone Repo

```bash
git clone https://github.com/armanmass/order_book.git
```

### Step 2: Prerequisites

Ensure you have a C++23 compatible compiler, CMake (4.10+), and conan installed.

### Step 3: Build the Project

```bash
conan install .
cmake --preset conan-release
cmake --build --preset conan-release
```

### Step 4: Run the Tests

```bash
./build/Release/tests/OrderBookTests
```

Expected output:

```
[==========] Running 9 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 9 tests from OrderBookTest
[ RUN      ] OrderBookTest.IsEmptyInit
[       OK ] OrderBookTest.IsEmptyInit (0 ms)
[ RUN      ] OrderBookTest.AddSingleOrder
[       OK ] OrderBookTest.AddSingleOrder (0 ms)
[ RUN      ] OrderBookTest.CancelSingleOrder
[       OK ] OrderBookTest.CancelSingleOrder (0 ms)
[ RUN      ] OrderBookTest.CancelNonExisting
[       OK ] OrderBookTest.CancelNonExisting (0 ms)
[ RUN      ] OrderBookTest.OrderIdConflict
[       OK ] OrderBookTest.OrderIdConflict (0 ms)
[ RUN      ] OrderBookTest.SingleMatch
[       OK ] OrderBookTest.SingleMatch (0 ms)
... etc ...
[----------] 9 tests from OrderBookTest (0 ms total)

[----------] Global test environment tear-down
[==========] 9 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 9 tests.
```

## Usage


Example usage:

```cpp
#include "OrderBook.hpp"

int main(int argc, char** argv)
{
    // 1. Create an OrderBook instance
    OrderBook book;

    // 2. Create a new GTC Buy order
    auto buyOrder = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)1,
        Side::Buy,
        (Price)100,
        (Quantity)10
    );

    // 3. Add the order to the book
    book.addOrder(buyOrder);
    std::cout << "Order book now contains " << book.size() << " order(s)." << std::endl;

    // 4. Create a matching Sell order
    auto sellOrder = std::make_shared<Order>(
        OrderType::GoodTillCancel,
        (OrderId)2,
        Side::Sell,
        (Price)100,
        (Quantity)5
    );

    // 5. Add the sell order, which will trigger a match
    Trades trades = book.addOrder(sellOrder);

    // 6. Check the results
    const auto& trade = trades.front();
    // TradeInfo.h for trade information interface

    return 0;
}
```
