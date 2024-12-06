Static vs. Dynamic Memory
=========================

In your C program you have to decide where the memory you want to use comes from.
There are two ways to get memory in your C code:

1. Define static memory.
2. Use dynamic memory (call `malloc()`/`free()` to get memory from the heap).

Both ways have some drawbacks which are listed here.
If you want to analyze the static memory consumption of your code you can use [otm](https://github.com/LudwigOrtmann/otm) or `make cosy`.

Static memory
-------------
* Access the memory in one operation O(1) ⇒ real time condition
* Programmer needs to know the amount of memory on compile time
  * Leads to over and undersized buffers
* Forces the programmer to think about the amount of need memory at compile time

Dynamic memory
--------------
* `malloc()` and `free()` are implemented in your `libc` (RIOT on ARM: `newlib`/`picolib`)
  * Runtime behavior is not predictable
* Code can request the amount of memory it needs on runtime
* On most platforms: the size of the heap is `sizeof(<RAM>)-sizeof(<static memory>)`
  * If you reduce your usage of static memory your heap gets bigger
* On some platforms calling  `free()` will not or not always make heap memory available again (see @ref oneway_malloc on MSP430)
* Programmer needs to handle failed memory allocation calls at runtime
* Static code analysis is unable to find errors regarding memory management
