#include "Item.h"
#include <iostream>

using namespace std;
void Item::printItem()
{
	cout << "Item: " << name << endl;
	cout << "Price: " << price << endl;
	cout << "Average 24 Hour Price: " << avg24Price << endl;
	cout << "Average 7 Days Price: " << avg7Price << endl;
	cout << "Sell to Trader: " << trader << endl;
	cout << "Trader Sell Price: " << buyBackPrice << endl;
	cout << "\n\n";
}