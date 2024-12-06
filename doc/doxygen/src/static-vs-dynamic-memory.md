This page discusses the usage of memory on restricted devices
=============================================================

In your c program you have to decide where the memory you want to use comes from.
There are two ways to get memory in your c code:

1. define static memory
2. use dynamic memory (call malloc/free to get memory from the heap)

Both ways have some drawbacks which are listed here.
If you want to analyze the static memory consumption of your code you can use [otm](https://github.com/LudwigOrtmann/otm).

static memory
-------------
* access the memory in one operation O(1) -> real time condition
* programmer needs to know the amount of memory on compile time
  * leads to over and undersized buffers
* forces the programmer to think about the amount of need memory at compile time

dynamic memory
--------------
* malloc and free are implemented in your libc (RIOT on ARM: newlib)
  * run time behavior not predictable
* code can request the amount of memory it needs on run time
* on most platforms: the size of the heap is sizeof(RAM)-sizeof(static memory)
  * if you reduce your usage of static memory your heap gets bigger
* on some platforms calling  `free()` will not or not always make heap memory available again (see onetime_malloc() on MSP430 or the implementation of sbrk() for lpc2387)
* programmer needs to handle failed memory allocation calls at runtime
* static code analysis is unable to find errors regarding memory management