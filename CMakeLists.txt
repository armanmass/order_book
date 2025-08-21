cmake_minimum_required(VERSION 4.1)

project(OrderBook)

add_library(
    OrderBookLib
    src/OrderBook.cpp
)

target_include_directories(
    OrderBookLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(
    main_exe
    src/main.cpp
)

target_link_libraries(
    main_exe
    PRIVATE
    OrderBookLib
)

enable_testing()
add_subdirectory(tests)