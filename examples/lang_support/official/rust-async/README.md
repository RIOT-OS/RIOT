Rust: Asynchronous programming
==============================

This is an example of how asynchronous Rust applications can be written on RIOT OS.

The application starts an [embassy] based executor,
and then spawn several tasks on the main thread.
Unlike RIOT OS threads, these tasks can not preempt each other;
they relinquish control of the main thread when performing an asynchronous operation that would block,
and are paused by anything that preempts the main thread.

Behind the scenes, the Rust compiler turns the asynchronous methods into state machines,
and the executor polls the state machines to make progress whenever an event wakes them up.

Upsides of this style of programming are that there is less need for synchronization
(for individual tasks or their parts can rely on the work they do synchronously not to be executed concurrently with other tasks if so configured),
and that they can share a single stack space.
State that is persisted across await points is not stored on the stack,
but in adequately sized static memory for each task that can run.

[embassy]: https://embassy.dev/
