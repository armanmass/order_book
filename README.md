# Order Book

A high-performance, multi-threaded C++20 implementation of a financial limit order book.

## Building and Running

### Step 1: Clone Repo

```bash
git clone https://github.com/your-username/OrderBook.git
cd OrderBook
```

### Step 2: Prerequisites

Ensure you have a C++20 compatible compiler (like GCC 13+ or Clang 18+) and CMake (3.10+) installed.

### Step 3: Build the Project

This will build the `OrderBookLib` static library and the `OrderBookTests` executable.

```bash
mkdir build
cd build
cmake ..
make
```

### Step 4: Run the Tests

After a successful build, you can run the unit tests to verify all functionality is working as expected.

```bash
# From the 'build' directory
./tests/OrderBookTests
```

You should see output indicating that all tests have passed:

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

This project builds a library that you can link against in your own C++ applications.

Here is an example of how to include `OrderBook` in your project.

**`my_app.cpp`:**

```cpp
#include "OrderBook.h"

int main() {
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
