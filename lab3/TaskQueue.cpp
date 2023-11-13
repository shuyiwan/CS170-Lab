#include "TaskQueue.h"

TaskQueue::
TaskQueue()
{
    // TODO: Your code here.
    // initilize the mutex and conditional var
    smutex_init(&this->mutex);
    scond_init(&this->condi);

}

TaskQueue::
~TaskQueue()
{
    // TODO: Your code here.
    // free mutex & conditional var
    smutex_destroy(&this->mutex);
    scond_destroy(&this->condi);
}

/*
 * ------------------------------------------------------------------
 * size --
 *
 *      Return the current size of the queue.
 *
 * Results:
 *      The size of the queue.
 *
 * ------------------------------------------------------------------
 */
int TaskQueue::
size()
{
    // TODO: Your code here.
    // return the size of the current queue
    smutex_lock(&this->mutex);
    int size = this->waitQueue.size();
    smutex_unlock(&this->mutex);
    return size; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * empty --
 *
 *      Return whether or not the queue is empty.
 *
 * Results:
 *      The true if the queue is empty and false otherwise.
 *
 * ------------------------------------------------------------------
 */
bool TaskQueue::
empty()
{
    // TODO: Your code here.
    smutex_lock(&this->mutex);
    bool is_empty = this->waitQueue.empty();
    smutex_unlock(&this->mutex);
    return is_empty; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * enqueue --
 *
 *      Insert the task at the back of the queue.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void TaskQueue::
enqueue(Task task)
{
    // TODO: Your code here.
    // push task into queue and singal the corresponding thread
    smutex_lock(&this->mutex);
    this->waitQueue.push(task);
    scond_signal(&this->condi, &this->mutex);
    smutex_unlock(&this->mutex);
}

/*
 * ------------------------------------------------------------------
 * dequeue --
 *
 *      Remove the Task at the front of the queue and return it.
 *      If the queue is empty, block until a Task is inserted.
 *
 * Results:
 *      The Task at the front of the queue.
 *
 * ------------------------------------------------------------------
 */
Task TaskQueue::
dequeue()
{
    // TODO: Your code here.
    smutex_lock(&this->mutex);
    // block till a task is inserted
    while (this->waitQueue.empty())
    {
        scond_wait(&this->condi, &this->mutex);
    }
    Task front_task = this->waitQueue.front();
    this->waitQueue.pop();
    smutex_unlock(&this->mutex);
    return front_task; // Keep compiler happy until routine done.
}

