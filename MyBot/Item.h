#include <string>

class Item
{
public:
    void printItem();

    // UID is ignored for now
    std::string name;
    int price;
    int avg24Price;
    int avg7Price;
    std::string trader;
    int buyBackPrice;
    std::string currency;
};