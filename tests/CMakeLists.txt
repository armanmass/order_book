find_package(GTest REQUIRED)

add_executable
(
    OrderBookTests
    OrderBook_test.cpp
)

target_link_libraries
(
    OrderBookTests
    PRIVATE
    main_executable
)

target_link_libraries
(
    OrderBookTests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)