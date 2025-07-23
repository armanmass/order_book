#include "OrderBook.h"
#include <iostream>

int main(int argc, char** argv)
{
    OrderBook ob;
    const OrderId oid = 1;
    ob.addOrder(std::make_shared<Order>(Order(OrderType::GoodTillCancel, oid, Side::Buy, 100, 10)));
    std::cout << ob.size() << std::endl;
    ob.cancelOrder(oid);
    std::cout << ob.size() << std::endl;

    return 0;
}