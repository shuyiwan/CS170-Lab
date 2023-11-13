#include "RequestHandlers.h"
#include "Request.h"
#include "EStore.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
using namespace std;
/*
 * ------------------------------------------------------------------
 * add_item_handler --
 *
 *      Handle an AddItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
add_item_handler(void *args)
{
    // TODO: Your code here.
    // cout << "1" << endl;
    AddItemReq *add_item_request = (AddItemReq *)args;
    // passing parameter to execute addItem
    int item_id = add_item_request->item_id;
    int quantity = add_item_request->quantity;
    double price = add_item_request->price;
    double discount = add_item_request->discount;
    add_item_request->store->addItem(item_id, quantity, price, discount); 
    
    //output
    cout << "Handling AddItemReq: item_id - " << item_id << ", quantity - " << quantity
        << ", price - $" << fixed << setprecision(2) << price
        << ", discount - " << discount << endl;

    delete add_item_request;
}

/*
 * ------------------------------------------------------------------
 * remove_item_handler --
 *
 *      Handle a RemoveItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
remove_item_handler(void *args)
{
    // TODO: Your code here.
    // cout << "2" << endl;
    RemoveItemReq *remove_item_request = (RemoveItemReq *)args;
    // passing parameter to remove an item
    int item_id = remove_item_request->item_id;
    remove_item_request->store->removeItem(item_id);
    // output
    cout << "Handling RemoveItemReq: item_id - " << item_id << endl;

    delete remove_item_request;
}

/*
 * ------------------------------------------------------------------
 * add_stock_handler --
 *
 *      Handle an AddStockReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
add_stock_handler(void *args)
{
    // TODO: Your code here.
    // cout << "3" << endl;
    AddStockReq *add_stock_request = (AddStockReq *)args;
    //pass item-id and stock to update the estore
    int item_id = add_stock_request->item_id;
    int additional_stock = add_stock_request->additional_stock;
    add_stock_request->store->addStock(item_id, additional_stock);

    // output
    cout << "Handling AddStockReq: item_id - " << item_id << ", additional_stock - " << additional_stock << endl;
    delete add_stock_request;
}

/*
 * ------------------------------------------------------------------
 * change_item_price_handler --
 *
 *      Handle a ChangeItemPriceReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_price_handler(void *args)
{
    // TODO: Your code here.
    // cout << "4" << endl;
    ChangeItemPriceReq *change_item_price_request = (ChangeItemPriceReq *)args;
    //pass item-id and new price to update the item in estore
    int item_id = change_item_price_request->item_id;
    double new_price = change_item_price_request->new_price;
    change_item_price_request->store->priceItem(item_id, new_price);
    //output
    cout << "Handling ChangeItemPriceReq: item_id - "
        << item_id << ", new_price - $" << fixed << setprecision(2) << new_price << endl;
    
    delete change_item_price_request;
}

/*
 * ------------------------------------------------------------------
 * change_item_discount_handler --
 *
 *      Handle a ChangeItemDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_discount_handler(void *args)
{
    // TODO: Your code here.
    // cout << "5" << endl;
    ChangeItemDiscountReq *change_item_discount_request = (ChangeItemDiscountReq *)args;
    //pass item-id and new discount to update discount in estore
    int item_id = change_item_discount_request->item_id;
    double new_discount = change_item_discount_request->new_discount;
    change_item_discount_request->store->discountItem(item_id, new_discount);
    //output 
    cout << "Handling ChangeItemDiscountReq: item_id - " << item_id
        << ", new_discount - " << fixed << setprecision(2) << new_discount << endl;

    delete change_item_discount_request;
}

/*
 * ------------------------------------------------------------------
 * set_shipping_cost_handler --
 *
 *      Handle a SetShippingCostReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
set_shipping_cost_handler(void *args)
{
    // TODO: Your code here.
    // cout << "6" << endl;
    SetShippingCostReq *set_shipping_cost_request = (SetShippingCostReq *)args;
    //pass new cost to update estore
    double new_cost = set_shipping_cost_request->new_cost;
    set_shipping_cost_request->store->setShippingCost(new_cost);
    //output
    cout << "Handling SetShippingCostReq: new_cost - $"
        << fixed << setprecision(2) << new_cost << endl;
    
    delete set_shipping_cost_request;
}

/*
 * ------------------------------------------------------------------
 * set_store_discount_handler --
 *
 *      Handle a SetStoreDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
set_store_discount_handler(void *args)
{
    // TODO: Your code here.
    // cout << "7" << endl;
    SetStoreDiscountReq *set_store_discount_request = (SetStoreDiscountReq *)args;
    //pass new discount to update estore
    double new_discount = set_store_discount_request->new_discount;
    set_store_discount_request->store->setStoreDiscount(new_discount);

    // output
    cout << "Handling SetStoreDiscountReq: new_discount - "
        << fixed << setprecision(2) << new_discount << endl;

    delete set_store_discount_request;
}

/*
 * ------------------------------------------------------------------
 * buy_item_handler --
 *
 *      Handle a BuyItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_item_handler(void *args)
{
    // TODO: Your code here.
    // cout << "8" << endl;
    BuyItemReq *buy_item_request = (BuyItemReq *)args;
    // pass itemid and budget to buy iten in store
    int item_id = buy_item_request->item_id;
    double budget = buy_item_request->budget;
    buy_item_request->store->buyItem(item_id, budget);

    //output
    cout << "Handling BuyItemReq: item_id - " << item_id
        << ", budget - $" << fixed << setprecision(2) << budget << endl;

    delete buy_item_request;
}

/*
 * ------------------------------------------------------------------
 * buy_many_items_handler --
 *
 *      Handle a BuyManyItemsReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_many_items_handler(void *args)
{
    // TODO: Your code here.
    // cout << "9" << endl;
    BuyManyItemsReq *buy_many_items_request = (BuyManyItemsReq *)args;
    // pass all the items to buy and budget
    vector<int>* item_ids = &buy_many_items_request->item_ids;
    double budget = buy_many_items_request->budget;
    buy_many_items_request->store->buyManyItems(item_ids, budget);
    //output
    cout << "Handling BuyManyItemsReq: item_ids - ";
    for (int i = 0; i < item_ids->size(); i++)
    {
        int item_id = item_ids->data()[i];
        cout << item_id << ", ";
    }
    cout << "budget - $" << fixed << setprecision(2) << budget << endl;

    delete buy_many_items_request;
}

/*
 * ------------------------------------------------------------------
 * stop_handler --
 *
 *      The thread should exit.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
stop_handler(void* args)
{
    // TODO: Your code here.
    // cout << "10" << endl;
    cout << "Handling StopHandlerReq: Quitting.\n";
    //exit
    sthread_exit();
}

