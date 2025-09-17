---
title: Hints for quicker & better RIOT development
description: Best practices and recommended tools for RIOT development
---

* Use the [methodology](#methodology-emulator-first-target-iot-hardware-last) described below.
* Use [`ccache`](/build-system/advanced_build_system_tricks/#speed-up-builds-with-ccache) to speedup compilation

## Coding "Dos" and "Don'ts"

### Dos
 * Use static memory. See also [Static vs. Dynamic Memory](#static-vs-dynamic-memory).
 * Select the priorities carefully.
 * Minimize stack usage with `DEVELHELP` and `CREATE_STACKTEST`.
 * Use threads to increase flexibility, modularity and robustness by leveraging IPC.
 * Use unsigned or signed integer (`unsigned`, `int`, `size_t` or `ssize_t`) for loop variables wherever possible,
   but keep in mind that on some platforms an `int` has a width of only 16-bit. In general, you should avoid types
   like `uint8_t` for loop iterators as they will probably make it more expensive on some platforms.
 * Join and factor out parts of the code with existing code in RIOT, where it makes sense.
 * Check all `size/length` parameters when passing memory, e.g. using `sizeof(x)` or `strlen(x)` as appropriate.
   Make sure you don't use the wrong one with a pointer.
 * Make sure all code paths can be reached. Make sure there are no always `true/false` conditions.
 * Make sure all critical sections (`lock/unlock`, `acquire/release`, ...) are always closed on every code path.
 * Make sure return values are consistent with our API documentation.
 * Use `assert()` statements to check parameters rather than returning an error code at run-time,
   to keep the code size down.
 * Use the `DEBUG(...)` macro rather than `log_x(...)`
 * Declare all internal module variables and functions `static`
 * Make sure variables are reduced in scope as much as possible
 * Use an appropriate signedness in your variables
 * Make sure the variables are big enough to prevent overflow. Be aware that the code may run on platforms with
   different sizes of variables. For example, `int/unsigned` is only 16-bit on msp430 and avr8. If in doubt,
   use portable types.
 * Reduce the number of function calls as far as possible without duplicating code.
 * Use good judgement when using `static inline` functions and macros. If they are used in multiple places,
   is the increase in performance worth the penalty in code size?
 * Use memory judiciously in general. For example:
```c
typedef enum {
    A,
    B,
    ...
} foo_t;

int bar(foo_t v)
{
    int abc;
    ...

    switch(v) {
    case A:
        abc = 23;
        break;
    case B:
        abc = 42;
        break;
    ...
    }
    ...
}

/* VS */

typedef enum {
    A = 23,
    B = 42,
    ...
} foo_t;

int bar(foo_t v) {
    int abc = v;
    ...
}
```

### Don'ts
 * Don't use too many threads. Try not to use more than one thread per module. Don't create threads for one-time tasks.
 * Don't use the POSIX wrapper if implementing something from scratch.
 * Don't allocate big chunks of memory (for instance the IPC message queue) on the stack,
   but use rather static memory for that.
 * Don't over-provision memory.
 * Don't pass stack memory between different contexts unless you can prove conclusively that it won't be a problem.
 * Don't use enums for flags, because flags have a width in memory that is in most cases smaller than `sizeof(enum)`
   (most bitfields are 16 bits max, on most of our newer platforms, `sizeof(enum)` is however 32 bits).
   This results in every assignment needed to be cast to either `uint8_t` or `uint16_t`. With macros you don't need to
   cast since they are typeless. Making the enum packed makes its width unpredictable in terms of alignment issues,
   when used in struct.
 * Don't duplicate code from elsewhere in the RIOT code base, unless there is a very good reason to do so.
 * Don't duplicate code within your own code, unless there is a very good reason to do so.
   Use internal functions to this end.
 * Don't mix up logical and bitwise operations (`!` vs `~`, or `&&` vs `&`)

## Methodology: emulator first, target IoT hardware last!

The below methodology is recommended, using well-known de facto standard tools from the FLOSS community that are
compatible with RIOT. Using the below workflow improves time-to-running-code compared to typical IoT software
workflows (which can be as retro as "LED-driven" debugging).

0. For newbies, preliminaries are typically faster with the provisioned virtual environment setup, e.g. with **Vagrant**.
1. To check your code, first use available **static analysis** as much as possible initially, which means
   (i) enable all compiler warnings and fix all problems found, then (ii) use a supported linter such as **cppcheck**
   to find bad coding patterns (i.e. code smells) and identify misuse of standard APIs.
2. Next, use available **dynamic analysis** tools to find further defects while running the code on **RIOT native**,
   which means (i) running unit tests and integration tests on RIOT native emulator, and (ii) using **Valgrind** memcheck,
   as well as the **GCC stack smashing detection**, to detect and avoid undefined behavior due to invalid memory access.
3. In case of networked applications or protocols, test **several instances of native** communicating via a virtual
   network mimicking the targeted scenario, which means (i) either using the default virtual full-mesh or other
   topologies configured via DESvirt, and (ii) using **Wireshark** to capture and analyze virtual network traffic,
   e.g. to ensure protocol packets are syntactically correct, and to observe network communication patterns.
4. In case of incorrect behavior at this stage, analyze the system state for semantic errors on native using the
   standard debugger **gdb**, which allows virtually unlimited conditional breakpoints, record and replay,
   catchpoints, tracepoints and watchpoints.
5. In case of suspected performance bottleneck, use performance profilers **gprof**, or else cachegrind,
   to identify precisely the bottlenecks.
6. At this stage the implementation has proven bug-free on the native emulator. One can thus finally move on to
   hardware-in-the-loop, which means (i) flashing the binary on the targeted IoT hardware, typically using
   standard flasher **OpenOCD** or **edbg**, and (ii) using the **RIOT shell** running on the target IoT device(s)
   for easier debugging on the target hardware.
7. In case the hardware is not available on-site, one can consider remotely flashing and testing the binary on
   supported open-access testbeds, e.g. [IoT-LAB](https://www.iot-lab.info) hardware is fully supported by RIOT.
8. In case of failure, after analyzing the failure and attempting to fix the defect, go back to step 1 to make sure
   the fix did not itself introduce a new defect.

## Static vs. Dynamic Memory

In your C program you have to decide where the memory you want to use comes from.
There are two ways to get memory in your C code:

1. Define static memory.
2. Use dynamic memory (call `malloc()`/`free()` to get memory from the heap).

Both ways have some drawbacks which are listed here.
If you want to analyze the static memory consumption of your code you can use
[otm](https://github.com/LudwigOrtmann/otm) or `make cosy`.

### Static memory
* Access the memory in one operation O(1) ⇒ real time condition
* Programmer needs to know the amount of memory on compile time
  * Leads to over and undersized buffers
* Forces the programmer to think about the amount of need memory at compile time

### Dynamic memory
* `malloc()` and `free()` are implemented in your `libc` (RIOT on ARM: `newlib`/`picolib`)
  * Runtime behavior is not predictable
* Code can request the amount of memory it needs on runtime
* On most platforms: the size of the heap is `sizeof(<RAM>)-sizeof(<static memory>)`
  * If you reduce your usage of static memory your heap gets bigger
* On some platforms calling  `free()` will not or not always make heap memory available again
  (see [Oneway Malloc on MSP430](https://doc.riot-os.org/group__oneway__malloc.html))
* Programmer needs to handle failed memory allocation calls at runtime
* Static code analysis is unable to find errors regarding memory management
