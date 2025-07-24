cmake_minimum_required(VERSION 3.10)

project(OrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    main_executable
    src/main.cpp
)

target_link_libraries(
    main_executable
    PRIVATE
    OrderBookLib
)

enable_testing()
add_subdirectory(tests)