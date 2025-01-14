IPC Pingpong!
=============

This example is to illustrate the usage of RIOTs IPC messaging system.

The application starts a second thread (in addition to the main thread) and sends messages between
these two threads. The main thread calls `thread_send_receive()` in an endless loop. The second
thread receives the message, prints `2nd: got msg from x` to stdout and sends a reply message with
an incrementing number back to the main thread. The main thread then prints the number it received
from the 2nd thread.

The correct output should look like this:
```
This is RIOT! (Version: xxx)
kernel_init(): jumping into first task...
Starting IPC Ping-pong example...
1st thread started, pid: 1
2nd thread started, pid: 2
2nd: Got msg from 1
1st: Got msg with content 2
2nd: Got msg from 1
1st: Got msg with content 3
2nd: Got msg from 1
1st: Got msg with content 4
2nd: Got msg from 1
1st: Got msg with content 5
2nd: Got msg from 1
1st: Got msg with content 6
2nd: Got msg from 1
1st: Got msg with content 7
2nd: Got msg from 1
1st: Got msg with content 8
2nd: Got msg from 1
1st: Got msg with content 9
2nd: Got msg from 1
1st: Got msg with content 10
[...]
```
