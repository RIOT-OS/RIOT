---
title: Working with Event Queues
description: This tutorial provides a quick introduction to using event queues in RIOT OS.
code_folder: examples/guides/event_queue/
---

## Introduction
An event queue is a FIFO (First In First Out) data structure that holds events
to be processed by an event handler.

Each event has exactly one callback function called the `handler`,
that is executed when the event is processed.
Posting an event to a queue triggers the execution of the event's handler
on the thread that owns the event queue.
Posting an event to a queue can be done from any thread or interrupt context.

This guide will give a quick introduction to using event queues in RIOT OS.
We will cover the following topics:
- [Regular Events](#regular-events)
- [Custom Events](#custom-events)
- [Handling Events](#handling-events)
- [Periodic Events](#periodic-events)

:::note
The whole source code for this guide can be found
[HERE](https://github.com/RIOT-OS/RIOT/tree/master/examples/guides/event_queue).

If your project is not working as expected,
you can compare your code with the code in this repository to see if you missed anything.
:::

### Regular Events
Creating and posting regular events is straightforward in RIOT.
First, we define the event handler function:
```c title="main.c"
static void regular_handler(event_t *event)
{
    (void) event;
    printf("\tTriggered regular event.\n");
}
```
Then, we can create an instance of the event:
```c title="main.c"
static event_t          event        = { .handler = regular_handler };
```
This event can now be posted to an event queue:
```c title="main.c"
    event_post(&my_queue, &event);
```
:::tip
See [Handling Events](#handling-events) for information about setting up an event queue.
:::
### Custom Events
Custom events allow us to pass additional data to the event handler.
To create a custom event, we need to define a new struct that contains
an `event_t` member and any additional data we want to pass.
For example, let's create a custom event that passes a string message:
```c time="main.c"
typedef struct {
    event_t super;
    const char *text;
} custom_event_t;
```
Next, we define the event handler function for the custom event:
```c title="main.c"
static void custom_handler(event_t *event)
{
    /* The handler receives a pointer to the base event structure.
     * We need to get the pointer to our custom event structure.
     */
    custom_event_t *custom_event = container_of(event, custom_event_t, super);
    printf("\tTriggered custom event with text: \"%s\".\n", custom_event->text);
}
```
Notice how we do not get passed the `custom_event_t` pointer directly,
but rather the embedded `event_t` pointer.
To access the additional fields of our custom event, we use the `container_of`
macro to access the parent structure.

Then, we can create an instance of our custom event:
```c title="main.c"
static custom_event_t   custom_event = { .super.handler = custom_handler,
                                         .text = "CUSTOM EVENT" };
```

Posting a custom event means posting the embedded `event_t` member:
```c title="main.c"
    event_post(&my_queue, &custom_event.super);
```
:::tip
See [Handling Events](#handling-events) for information about setting up an event queue.
:::
### Handling Events
It is common to have a dedicated thread for handling events.
A simple thread function that processes incoming events looks like this:
```c title="main.c"
void *event_handler_thread(void *arg)
{
    assert(arg != NULL);
    event_queue_t *queue = (event_queue_t *)arg;

    /* A thread must own an event queue to process its events.
     * Ownership is initially assigned to the thread that initializes the queue.
     * It can later be transferred by calling `event_queue_claim`.
     */
    event_queue_init(queue);

    puts("Event handler thread listening for events...");
    event_loop(queue);
    return NULL;
}
```
:::note
The thread calling `event_queue_init` becomes the owner of the event queue. Only the owner can
process events from the queue. The ownership can be transferred to another thread using the
`event_queue_claim` function.
:::
Once the thread is started and has initialized the event queue, we can post events to it
from any other thread or interrupt context.
The handler functions of the posted events will be executed on the event handler thread.

Since using a dedicated thread for handling events is common,
RIOT provides a module which sets up event queues and a handler thread for us.

To use it, add the module to your application's Makefile:
```makefile
USEMODULE += event_thread
```

Now we can post to the `EVENT_PRIO_HIGHEST`, `EVENT_PRIO_MEDIUM`, and `EVENT_PRIO_LOWEST` queues.
A single thread will be automatically created that handles events posted to the queues
according to their priority.
Posting an event to one of these queues is as simple as:
```c title="main.c"
    event_post(EVENT_PRIO_MEDIUM, &event);
```
## Periodic Events
Periodic events are events that are posted to an event queue at regular intervals.
First we need to include the module in our application's Makefile:
```makefile
USEMODULE += event_periodic
```

To create a periodic event, we need to declare and initialize a `event_periodic_t` structure.
```c title="main.c"
    /* This initializes the periodic event. We set the timer it should use,
     * the queue it should post to, and the event it should post.
     */
    event_periodic_init(&periodic_event, ZTIMER_SEC, EVENT_PRIO_MEDIUM, &event);
```
Once the periodic event is initialized, we can start it:
```c title="main.c"
    event_periodic_start(&periodic_event, 1);
```
:::note
A periodic event can be set to repeat only a certain number of times using the
`event_periodic_set_count` function.
:::