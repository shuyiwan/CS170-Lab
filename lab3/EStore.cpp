#include <cassert>
#include <iostream>
#include "EStore.h"
using namespace std;

Item::
Item() : valid(false)
{
}

Item::
~Item()
{
}


EStore::
EStore(bool enableFineMode)
    : fineMode(enableFineMode)
{
    // TODO: Your code here.
    // initialization
    smutex_init(&this->mutex);
    scond_init(&this->condivar);
    this->store_discount = 0;
    this->shipping_cost = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        this->inventory[i].discount = 0;
        this->inventory[i].price = 0;
        this->inventory[i].quantity = 0;
        this->inventory[i].valid = false;
    }
    if (this->fineMode)
    {
        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            smutex_init(&this->mutexes[i]);
        }
    }

}

EStore::
~EStore()
{
    // TODO: Your code here.
    smutex_destroy(&this->mutex);
    scond_destroy(&this->condivar);
    if (this->fineMode)
    {
        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            smutex_destroy(&this->mutexes[i]);
        }
    }
}

/*
 * ------------------------------------------------------------------
 * buyItem --
 *
 *      Attempt to buy the item from the store.
 *
 *      An item can be bought if:
 *          - The store carries it.
 *          - The item is in stock.
 *          - The cost of the item plus the cost of shipping is no
 *            more than the budget.
 *
 *      If the store *does not* carry this item, simply return and
 *      do nothing. Do not attempt to buy the item.
 *
 *      If the store *does* carry the item, but it is not in stock
 *      or its cost is over budget, block until both conditions are
 *      met (at which point the item should be bought) or the store
 *      removes the item from sale (at which point this method
 *      returns).
 *
 *      The overall cost of a purchase for a single item is defined
 *      as the current cost of the item times 1 - the store
 *      discount, plus the flat overall store shipping fee.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyItem(int item_id, double budget)
{
    assert(!fineModeEnabled());
    
    // TODO: Your code here.
    double current_price = 0;
    double total_cost = 0;
    smutex_lock(&this->mutex);
    // if store doesn't carry the item
    if (!(this->inventory[item_id].valid))
    {
        smutex_unlock(&this->mutex);
        return;
    }

    // does carry, not in stock or short in budget
    // wait till both condi are met
    current_price = this->inventory[item_id].price * (1 - this->inventory[item_id].discount);
    total_cost = current_price * (1 - this->store_discount) + this->shipping_cost; 
    while ((this->inventory[item_id].quantity == 0) || (total_cost > budget)) 
    {
        scond_wait(&this->condivar, &this->mutex);
    }

    // buy item
    this->inventory[item_id].quantity--;
    smutex_unlock(&this->mutex);
}

/*
 * ------------------------------------------------------------------
 * buyManyItem --
 *
 *      Attempt to buy all of the specified items at once. If the
 *      order cannot be bought, give up and return without buying
 *      anything. Otherwise buy the entire order at once.
 *
 *      The entire order can be bought if:
 *          - The store carries all items.
 *          - All items are in stock.
 *          - The cost of the the entire order (cost of items plus
 *            shipping for each item) is no more than the budget.
 *
 *      If multiple customers are attempting to buy at the same
 *      time and their orders are mutually exclusive (i.e., the
 *      two customers are not trying to buy any of the same items),
 *      then their orders must be processed at the same time.
 *
 *      For the purposes of this lab, it is OK for the store
 *      discount and shipping cost to change while an order is being
 *      processed.
 *
 *      The cost of a purchase of many items is the sum of the
 *      costs of purchasing each item individually. The purchase
 *      cost of an individual item is covered above in the
 *      description of buyItem.
 *
 *      Challenge: For bonus points, implement a version of this
 *      method that will wait until the order can be fulfilled
 *      instead of giving up. The implementation should be efficient
 *      in that it should not wake up threads unecessarily. For
 *      instance, if an item decreases in price, only threads that
 *      are waiting to buy an order that includes that item should be
 *      signaled (though all such threads should be signaled).
 *
 *      Challenge: For bonus points, ensure that the shipping cost
 *      and store discount does not change while processing an
 *      order.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyManyItems(vector<int>* item_ids, double budget)
{
    assert(fineModeEnabled());

    // TODO: Your code here.
    double total_cost = 0;
    double curr_item_price = 0;
    for (int i = 0; i < item_ids->size(); i++)
    {
        int item_id = (*item_ids)[i];
        smutex_lock(&this->mutexes[item_id]);
        if (!(this->inventory[item_id].valid) || (this->inventory[item_id].quantity == 0))
        {
            //cannot buy cuz not in stock or invalid, just return
            smutex_unlock(&this->mutexes[item_id]);
            return;
        }
        curr_item_price = this->inventory[item_id].price * (1 - this->inventory[item_id].discount);
        total_cost += curr_item_price * (1 - this->store_discount) + this->shipping_cost;
        // cannot buy because short of budget
        if(budget < total_cost)
        {
            smutex_unlock(&this->mutexes[item_id]);
            return;
        }
        smutex_unlock(&this->mutexes[item_id]);
    }

    // make the order at once
    for (int i = 0; i < item_ids->size(); i++) 
    {
        int item_id = item_ids->data()[i];
        smutex_lock(&this->mutexes[item_id]);
        this->inventory[item_id].quantity--;
        smutex_unlock(&this->mutexes[item_id]);
    }
    
}

/*
 * ------------------------------------------------------------------
 * addItem --
 *
 *      Add the item to the store with the specified quantity,
 *      price, and discount. If the store already carries an item
 *      with the specified id, do nothing.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addItem(int item_id, int quantity, double price, double discount)
{
    // TODO: Your code here.
    smutex_t *p_mutex = NULL;
    if (this->fineMode) p_mutex = &this->mutexes[item_id];
    else p_mutex = &this->mutex;

    smutex_lock(p_mutex);

    // if store already carries item, no nothing
    if (this->inventory[item_id].valid) {
        smutex_unlock(p_mutex);
        return;
    }

    this->inventory[item_id].quantity = quantity;
    this->inventory[item_id].price = price;
    this->inventory[item_id].discount = discount;
    this->inventory[item_id].valid = true;

    scond_broadcast(&this->condivar, p_mutex);

    smutex_unlock(p_mutex);
}

/*
 * ------------------------------------------------------------------
 * removeItem --
 *
 *      Remove the item from the store. The store no longer carries
 *      this item. If the store is not carrying this item, do
 *      nothing.
 *
 *      Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
removeItem(int item_id)
{
    // TODO: Your code here.
    smutex_t *p_mutex = NULL;
    if (this->fineMode) p_mutex = &this->mutexes[item_id];
    else p_mutex = &this->mutex;

    smutex_lock(p_mutex);
    if (!(this->inventory[item_id].valid)) //check validation
    {
        smutex_unlock(p_mutex);
        return;
    }
    // set the validation to false, not carry anymore
    this->inventory[item_id].valid = false;
    smutex_unlock(p_mutex);
}

/*
 * ------------------------------------------------------------------
 * addStock --
 *
 *      Increase the stock of the specified item by count. If the
 *      store does not carry the item, do nothing. Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addStock(int item_id, int count)
{
    // TODO: Your code here.
    smutex_t *p_mutex = NULL;
    if (this->fineMode) p_mutex = &this->mutexes[item_id];
    else p_mutex = &this->mutex;

    smutex_lock(p_mutex);
    //check the validation 
    if (!(this->inventory[item_id].valid)) 
    {
        smutex_unlock(p_mutex);
        return;
    }
    //add stock and wake any waiters 
    this->inventory[item_id].quantity += count;
    scond_broadcast(&this->condivar, p_mutex);
    smutex_unlock(p_mutex);
}

/*
 * ------------------------------------------------------------------
 * priceItem --
 *
 *      Change the price on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item price decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
priceItem(int item_id, double price)
{
    // TODO: Your code here.
    smutex_t *p_mutex = NULL;
    if (this->fineMode) p_mutex = &this->mutexes[item_id];
    else p_mutex = &this->mutex;

    smutex_lock(p_mutex);
    //check the validation, return if item are not carried
    if (!(this->inventory[item_id].valid)) 
    {
        smutex_unlock(p_mutex);
        return;
    }
    // compare old price and new price
    // wake any waiters if has lower price
    double old_price = this->inventory[item_id].price;
    this->inventory[item_id].price = price;
    if (price < old_price)
    {
        scond_broadcast(&this->condivar, p_mutex);
    }
    smutex_unlock(p_mutex);
}

/*
 * ------------------------------------------------------------------
 * discountItem --
 *
 *      Change the discount on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item discount increased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
discountItem(int item_id, double discount)
{
    // TODO: Your code here.
    smutex_t *p_mutex = NULL;
    if (this->fineMode) p_mutex = &this->mutexes[item_id];
    else p_mutex = &this->mutex;
    // check the validation, return if item are not carried
    smutex_lock(p_mutex);
    if (!(this->inventory[item_id].valid)) 
    {
        smutex_unlock(p_mutex);
        return;
    }
    // compare the old diescount and new discount
    // wake any waiters if has increased discount
    double old_discount= this->inventory[item_id].discount;
    this->inventory[item_id].discount = discount;
    if (discount > old_discount)
    {
        scond_broadcast(&this->condivar, p_mutex);
    }
    smutex_unlock(p_mutex);
}

/*
 * ------------------------------------------------------------------
 * setShippingCost --
 *
 *      Set the per-item shipping cost. If the shipping cost
 *      decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setShippingCost(double cost)
{
    // TODO: Your code here.
    smutex_lock(&this->mutex);
    // compare new cost and old cost
    // if cost is lower wake any waiters
    double old_cost = this->shipping_cost;
    this->shipping_cost = cost;
    if (cost < old_cost) 
    {
        scond_broadcast(&this->condivar, &this->mutex);
    }
    smutex_unlock(&this->mutex);
}

/*
 * ------------------------------------------------------------------
 * setStoreDiscount --
 *
 *      Set the store discount. If the discount increased, wake any
 *      waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setStoreDiscount(double discount)
{
    // TODO: Your code here.
    smutex_lock(&this->mutex);
    // compare old discount and new discount
    // broadcast when has increased discount
    double old_discount = this->store_discount;
    this->store_discount = discount;
    if (discount > old_discount) 
    {
        scond_broadcast(&this->condivar, &this->mutex);
    }
    smutex_unlock(&this->mutex);
}


