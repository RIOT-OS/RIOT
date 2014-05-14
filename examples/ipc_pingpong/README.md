IPC Pingpong!
=============

This example is to illustrate the usage of RIOTs IPC messaging system.

The application starts a second thread (in addition to the main thread) and sends messages between
these two threads. The main thread call `thread_send_receive()` in an endless loop. The second
thread receives the message, prints `2nd: got msg from x` to stdout and sends a reply message with
an incrementing number back to the main thread.

The correct output should look like this:
```
kernel_init(): This is RIOT! (Version: xxx)
kernel_init(): jumping into first task...
Hello world!
second_thread starting.
2nd: got msg from 1
Got msg with content 2
2nd: got msg from 1
Got msg with content 3
2nd: got msg from 1
Got msg with content 4
2nd: got msg from 1
Got msg with content 5
2nd: got msg from 1
Got msg with content 6
2nd: got msg from 1
Got msg with content 7
2nd: got msg from 1
Got msg with content 8
2nd: got msg from 1
Got msg with content 9
2nd: got msg from 1
Got msg with content 10
2nd: got msg from 1
[...]
```
