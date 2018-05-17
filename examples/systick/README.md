examples/systick
================

This application runs 4 threads (tasks) using systick hardware timer for preemption
* thread 'test0' every second
* thread 'test1' every 2 seconds
* thread 'test2' every 3 seconds
* thread 'test3' never yield

With no-preempt scheduling (classic RIOT way of life), thread 'test3' would block infinitely.
With systick timer, isr_systick() launch thread_yield() every X ticks to schedule.

Usage
=====

Build, flash and start the application:
```
export BOARD=your_board
make
make flash
make term
```

Example output
==============

```
main(): This is RIOT! (Version: 2018.04-devel-373-g4647c-gdo-sfl-laptop-systick_example)
SUCCESS !
test0 - 1 seconds
test1 - 2 seconds
test2 - 3 seconds
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test0 - 1 seconds
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test0 - 1 seconds
test1 - 2 seconds
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test0 - 1 seconds
test2 - 3 seconds
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test0 - 1 seconds
test1 - 2 seconds
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test3 - Forever
test0 - 1 seconds
Received: p
*** RIOT kernel panic:
Debug panic to display threads

        pid | name                 | state    Q | pri | stack  ( used) | base addr  | current
          - | isr_stack            | -        - |   - |    512 (  140) | 0x20000000 | 0x200001c8
          1 | idle                 | pending  Q |  15 |    256 (  120) | 0x200003bc | 0x20000444
          2 | main                 | running  Q |   7 |   1536 (  528) | 0x200004bc | 0x200008ac
          3 | test0                | bl mutex _ |   8 |   1024 ( 1020) | 0x2000135c | 0x20001694
          4 | test1                | bl mutex _ |   9 |   1024 ( 1020) | 0x2000175c | 0x20001a94
          5 | test2                | bl mutex _ |  10 |   1024 ( 1020) | 0x20000b5c | 0x20000e94
          6 | test3                | pending  Q |  11 |   1024 ( 1020) | 0x20000f5c | 0x200012dc
            | SUM                  |            |     |   6400 ( 4868)

*** halted.
```
