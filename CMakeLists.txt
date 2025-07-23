cmake_minimum_required(VERSION 3.10)

project(OrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    main_executable
    src/main.cpp
    src/OrderBook.cpp
)

target_include_directories(
    main_executable 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)