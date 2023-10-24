#include "Main.h"
#include "Item.h"
#include <winsock2.h>
#include <windows.h>
#include <dpp/dpp.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#pragma comment(lib, "urlmon.lib")
using namespace std;
void refreshItems(std::vector<Item>& itemList) // Downloads new file and fills Item vector
{

	// URLDownloadToFile(NULL, L"GOOGLE DOCS LINK TO LATEST CSV", L"FILE LOCATION", 0, NULL);

	ifstream file("tarkov.csv");
	string stringInt;
	if (!file.is_open()) // Error check if file exists
		cout << "ERROR: File Not Found";
	while (file.peek() != EOF) // While it isn't the end of file
	{
		Item currentItem; // Create item to store values
		getline(file, currentItem.name, ','); // Read in name

		getline(file, stringInt, ','); // Read in current price
		try
		{
			currentItem.price = stoi(stringInt); // String to int
		}
		catch (exception e)
		{
			currentItem.price = 0;
		}
		getline(file, stringInt, ','); // Read in 24 hr price
		try
		{
			currentItem.avg24Price = stoi(stringInt);
		}
		catch (exception e)
		{
			currentItem.avg24Price = 0;
		}
		getline(file, stringInt, ','); // Read in 7 days price
		try
		{
			currentItem.avg7Price = stoi(stringInt);
		}
		catch (exception e)
		{
			currentItem.avg7Price = 0;
		}
		getline(file, currentItem.trader, ','); // Read in trader name
		getline(file, stringInt, ','); // Read in buy back price
		try
		{
			currentItem.buyBackPrice = stoi(stringInt);
		}
		catch (exception e)
		{
			currentItem.buyBackPrice = 0;
		}
		getline(file, currentItem.currency, '\n'); // Read in currency
		itemList.push_back(currentItem); // Add item to list
	}
}

bool instantReturns(Item thisItem, int dollarValue, int euroValue) // Returns true if reselling produces profit >800 (based on 24hprice
{
	if ((thisItem.price > 0) && (thisItem.currency != "$")) // NOT DOLLARS
	{
		if ((thisItem.price != thisItem.avg24Price) && (thisItem.price != thisItem.avg7Price))
		{
			if (thisItem.currency == "\xE2\x82\xBD") // Roubles in UTF-8
				return ((thisItem.buyBackPrice - thisItem.avg24Price) >= 800);
			else if (thisItem.currency == "$") // Dollars
				return (((thisItem.buyBackPrice * dollarValue) - thisItem.avg24Price) >= 800);
			else if (thisItem.currency == "\xE2\x82\xAC") // Euros in UTF-8
				return (((thisItem.buyBackPrice * euroValue) - thisItem.avg24Price) >= 800);
			else
			{
				return false;
				cout << "returned false\n";
			}
		}
	}
	return false;
}

void refreshCurrency(std::vector<Item> itemList, int& dollarValue, int& euroValue) // Finds the dollar and euro value from the Item vector
{
	for (auto item : itemList) {
		if (item.name == "Dollars")
			dollarValue = item.price;
		else if (item.name == "Euros")
			euroValue = item.price;
	}
}

//string estimatedProfit(Item item, int dollarValue, int euroValue)
//{
//	if (item.currency == "₽")
//		return (to_string(item.buyBackPrice - item.avg24Price));
//	else if (item.currency == "$")
//		return (to_string((item.buyBackPrice * dollarValue) - item.avg24Price));
//	else if (item.currency == "€")
//		return (to_string((item.buyBackPrice * euroValue) - item.avg24Price));
//}

int main() 
{
    // Setup the bot
    dpp::cluster bot("DISCORD API KEY", dpp::i_default_intents | dpp::i_message_content);

    // Message handler to look for a command called !embed
    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        if (event.msg.content == "!profit") {
			vector<Item> itemList;

			refreshItems(itemList);

			int dollarValue = 0;
			int euroValue = 0;

			refreshCurrency(itemList, dollarValue, euroValue); // Assigns exchange rate for other currencys
			cout << "dollarValue = " << dollarValue << ", euroValue = " << euroValue;

			for (auto item : itemList)
			{
				if (instantReturns(item, dollarValue, euroValue)) {
					dpp::embed embed = dpp::embed().
						set_color(dpp::colors::sti_blue).
						set_title(item.name).
						add_field(
							"Price",
							"\xE2\x82\xBD" + to_string(item.price) // UTF-8 rouble unicode
						).
						add_field(
							"24H Price",
							"\xE2\x82\xBD" + to_string(item.avg24Price),
							true
						).
						add_field(
							"7D Price",
							"\xE2\x82\xBD" + to_string(item.avg7Price),
							true
						).
						add_field(
							"Trader",
							item.trader,
							true
						).
						add_field(
							"Sell Price",
							item.currency + to_string(item.buyBackPrice),
							true
						).
						/*add_field(
							"Estimated Profit",
							"\xE2\x82\xBD" + estimatedProfit(item, dollarValue, euroValue),
							true
						).*/
						set_timestamp(time(0));

					/* reply with the created embed */
					bot.message_create(dpp::message(event.msg.channel_id, embed).set_reference(event.msg.id));
				}
			}       
        }
        });

    bot.start(dpp::st_wait);
    return 0;
}