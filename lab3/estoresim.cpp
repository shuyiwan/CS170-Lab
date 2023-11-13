#include <cstring>
#include <cstdlib>
#include <iostream>

#include "EStore.h"
#include "RequestGenerator.h"
#include "TaskQueue.h"

class Simulation
{
    public:
    TaskQueue supplierTasks;
    TaskQueue customerTasks;
    EStore store;

    int maxTasks;
    int numSuppliers;
    int numCustomers;

    explicit Simulation(bool useFineMode) : store(useFineMode) { }
};

/*
 * ------------------------------------------------------------------
 * supplierGenerator --
 *
 *      The supplier generator thread. The argument is a pointer to
 *      the shared Simulation object.
 *
 *      Enqueue arg->maxTasks requests to the supplier queue, then
 *      stop all supplier threads by enqueuing arg->numSuppliers
 *      stop requests.
 *
 *      Use a SupplierRequestGenerator to generate and enqueue
 *      requests.
 *
 *      This thread should exit when done.
 *
 * Results:
 *      Does not return. Exit instead.
 *
 * ------------------------------------------------------------------
 */
static void*
supplierGenerator(void* arg)
{
    // TODO: Your code here.
    Simulation *p_sim = (Simulation *)arg;
    SupplierRequestGenerator generator(&p_sim->supplierTasks);
    generator.enqueueTasks(p_sim->maxTasks, &(p_sim->store));
    generator.enqueueStops(p_sim->numSuppliers);
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * customerGenerator --
 *
 *      The customer generator thread. The argument is a pointer to
 *      the shared Simulation object.
 *
 *      Enqueue arg->maxTasks requests to the customer queue, then
 *      stop all customer threads by enqueuing arg->numCustomers
 *      stop requests.
 *
 *      Use a CustomerRequestGenerator to generate and enqueue
 *      requests.  For the fineMode argument to the constructor
 *      of CustomerRequestGenerator, use the output of
 *      store.fineModeEnabled() method, where store is a field
 *      in the Simulation class.
 *
 *      This thread should exit when done.
 *
 * Results:
 *      Does not return. Exit instead.
 *
 * ------------------------------------------------------------------
 */
static void*
customerGenerator(void* arg)
{
    // TODO: Your code here.
    Simulation *p_sim = (Simulation *)arg;
    CustomerRequestGenerator generator(&p_sim->customerTasks, p_sim->store.fineModeEnabled());
    generator.enqueueTasks(p_sim->maxTasks, &(p_sim->store));
    generator.enqueueStops(p_sim->numCustomers);
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * supplier --
 *
 *      The main supplier thread. The argument is a pointer to the
 *      shared Simulation object.
 *
 *      Dequeue Tasks from the supplier queue and execute them.
 *
 * Results:
 *      Does not return.
 *
 * ------------------------------------------------------------------
 */
static void*
supplier(void* arg)
{
    // TODO: Your code here.
    Simulation *p_sim = (Simulation *)arg;
    while (true)
    {
        Task task = p_sim->supplierTasks.dequeue();
        task.handler(task.arg);
    }
    
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * customer --
 *
 *      The main customer thread. The argument is a pointer to the
 *      shared Simulation object.
 *
 *      Dequeue Tasks from the customer queue and execute them.
 *
 * Results:
 *      Does not return.
 *
 * ------------------------------------------------------------------
 */
static void*
customer(void* arg)
{
    // TODO: Your code here.
    Simulation *p_sim = (Simulation *)arg;
    while(true)
    {
        Task task = p_sim->customerTasks.dequeue();
        task.handler(task.arg);
    }
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * startSimulation --
 *      Create a new Simulation object. This object will serve as
 *      the shared state for the simulation. 
 *
 *      Create the following threads:
 *          - 1 supplier generator thread.
 *          - 1 customer generator thread.
 *          - numSuppliers supplier threads.
 *          - numCustomers customer threads.
 *
 *      After creating the worker threads, the main thread
 *      should wait until all of them exit, at which point it
 *      should return.
 *
 *      Hint: Use sthread_join.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
static void
startSimulation(int numSuppliers, int numCustomers, int maxTasks, bool useFineMode)
{
    // TODO: Your code here.
    cout << "START---------------------\n";
    Simulation *simulation = new Simulation(useFineMode);
    simulation->numCustomers = numCustomers;
    simulation->maxTasks = maxTasks;
    simulation->numSuppliers = numSuppliers;

    sthread_t supplier_generator_t;
    sthread_t customer_generator_t;

    sthread_create(&supplier_generator_t, supplierGenerator, simulation);
    sthread_create(&customer_generator_t, customerGenerator, simulation);

    // creates the threads
    sthread_t suppliers[numSuppliers];
    sthread_t customers[numCustomers];
    for (int i = 0; i < numSuppliers; i++)
    {
        sthread_create(&suppliers[i], supplier, simulation);
    }
    for (int i = 0; i < numCustomers; i++)
    {
        sthread_create(&customers[i], customer, simulation);
    }

    // implement the threads
    sthread_join(supplier_generator_t);
    sthread_join(customer_generator_t);
    for (int i = 0; i < numSuppliers; i++)
    {
        sthread_join(suppliers[i]);
    }
    for (int i = 0; i < numCustomers; i++)
    {
        sthread_join(customers[i]);
    }

    cout << "ENDING----------------\n";
}

int main(int argc, char **argv)
{
    bool useFineMode = false;

    // Seed the random number generator.
    // You can remove this line or set it to some constant to get deterministic
    // results, but make sure you put it back before turning in.
    srand(time(NULL));

    if (argc > 1)
        useFineMode = strcmp(argv[1], "--fine") == 0;
    startSimulation(10, 10, 100, useFineMode);
    return 0;
}

