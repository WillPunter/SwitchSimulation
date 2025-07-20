/*  simulator.c

    The simulator needs to maintain an event queue and a current time. Events
    can be invoked for the future.
    
    The event queue should be implemented as a heap comprised of event
    structures. These event structures should store an event ID by which to
    index them in an event table, which contains the corresponding callback
    function to be executed.

    The simulation then simply runs in a loop, where it dequeues the soonest
    event and runs its corresponding callback with its provided argument.
     
    The main source of complexity */

struct event {
    unsigned int event_id;
    void *arg;
    void *time;
}