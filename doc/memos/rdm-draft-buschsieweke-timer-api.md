- RDM: 2
- Title: High Level Timer API: Survey of Common Features and Requirement Analysis
- Author: Alexandre Abadie, José Alamos, Emmanuel Baccelli, Marian Buschsieweke,
          Niels Gandraß, Martine Lenders, Michel Rottleuthner, Kaspar Schleiser,
          Thomas C. Schmidt, Benjamin Valentin,
- Status: Active
- Type: Consensus
- Created: October 2018

## Abstract

This document presents use cases, basic requirements, and goals for a high level
timer API to be used in RIOT.

## Status

This document is a product of the community of RIOT maintainers, and aims to
represent the consensus within this community. The content of this document is
licensed with a Creative Commons CC-BY-SA license.

## Terminology

This document uses the [RFC2119](https://www.ietf.org/rfc/rfc2119.txt)
terminology and the following acronyms:

- RTC: Real Time Clock
- RTT: Real Time Timer

Throughout this document, the following terms should be understood as:

- Real Time Clock: A low power hardware clock that represents time broken up
  into year, month, day, hour, minute, and second. It has a fixed resolution of
  1 second and provides all mandatory features of the [RTC API](http://api.riot-os.org/group__drivers__periph__rtc.html)
- Real Time Timer: A low power hardware clock that is very similar to the RTC,
  but operations on a time representation using a single integer. Unlike the
  RTC, the resolution can differ from 1 second. RTTs provide all mandatory
  features of the [RTT API](http://api.riot-os.org/group__drivers__periph__rtt.html)
- Software Timer: A mechanism to run code at a given point in time in the
  future. The number of concurrently usable software timers is only limited by
  their memory requirements and the load a system can handle.
- System Time: Common time reference of the local device, usually as a single
  integer counting the clock ticks passed since some reference point

# 1. Introduction

## Motivation

Many applications and use cases for using timer hardware for various things
ranging from measuring time, delaying the execution of a thread, or running
code at specific points in time exists. Convenience, portability and ease of
use are therefore essential for a user friendly embedded OS.

Actual timer hardware however is difficult to use. One main reason is that
timer hardware can differ significantly starting with the clock frequency,
the timer width (8 bit, 16 bit, 24 bit, and 32 bit are all common), and their
time representation. Another reason is that hardware timers only allow setting
a (very) small number of alarm callbacks.

A high level timer API is needed to provide convenient and portable access to
the hardware features and is essential when multiple virtual timers need to
be provided on top of a single hardware timer.

## Related Timer APIs and Commonly Provided Features

The following list of related APIs provided by other operating systems targeting
constraint devices of class C2 according to RFC 7228 terminology has been
studied:

- FreeRTOS:
    - [Software Timer Feature description](https://www.freertos.org/RTOS-software-timer.html)
    - [Software Timer API reference](https://www.freertos.org/FreeRTOS-Software-Timer-API-Functions.html)
    - [Task Control API reference](https://www.freertos.org/a00112.html)
    - [`xTaskGetTickCount()` API reference](https://www.freertos.org/a00021.html#xTaskGetTickCount)
- Zephyr:
    - [Timer API documentation](https://docs.zephyrproject.org/1.12.0/kernel/timing/timers.html)
    - [Kernel Clocks documentation](https://docs.zephyrproject.org/1.9.0/kernel/timing/clocks.html)
- Mbed OS:
    - [Timer API documentation](https://os.mbed.com/docs/mbed-os/v5.14/apis/timer.html)
    - [Ticker API documentation](https://os.mbed.com/docs/mbed-os/v5.14/apis/ticker.html)
    - [`wait()` API documentation](https://os.mbed.com/docs/mbed-os/v5.14/apis/wait.html)
- Contiki
    - [Wiki page on timers](https://github.com/contiki-os/contiki/wiki/Timers)
- TinyOS:
    - [Timer header file](https://github.com/tinyos/tinyos-main/blob/master/tos/lib/timer/Timer.nc)
    - [TOSThreads Tutorial](http://tinyos.stanford.edu/tinyos-wiki/index.php/TOSThreads_Tutorial)
- Mynewt:
    - [OS Time API reference](https://mynewt.apache.org/latest/os/core_os/time/os_time.html)
- Xtimer, RIOT's (at the time of writing) current timer API
    - [xtimer API documentation](http://api.riot-os.org/group__sys__xtimer.html)
- ztimer, RIOT's alternative timer API that aims to eventually replace xtimer
    - [ztimer API documentation](https://api.riot-os.org/group__sys__ztimer.html)

The following common feature sets provided by competing operating systems have
been identified:

- APIs related to Software Timers
    - A given callback function is executed after a specific point in time
      (in the future) has reached
    - Some OS support both one-shot timers and periodic timers
    - Depending on the OS callbacks are executed either in IRQ context or by
      a specific thread
- APIs related to Time Tracking
    - Getting the current system time
    - Quantify time spans between to events
- APIs used to delay the execution flow

| Feature                   | FreeRTOS      | Zephyr        | Mbed OS   | Contiki       | TinyOS    | Mynewt    | xtimer    | ztimer            |
|:------------------------- |:------------- |:------------- |:--------- |:------------- |:--------- |:--------- |:--------- |:----------------- |
| Software Timers           | ✓             | ✓             | ✓         | ✓             | ✓         | ✗         | ✓         | ✓                 |
| Periodic Software Timers  | ✓             | ✓             | ✓         | ● (2)         | ✓         | ✗         | ● (6)     | ● (6)             |
| Context of Timer Callback | Thread        | ?             | ?         | Both? (3)     | ?         | n.a.      | Interrupt | Interrupt         |
| System Time               | ✓             | ✓             | ✓         | ✓             | ✓         | ✓         | ✓         | ● (8)             |
| System Time Width         | `TickType_t`  | 32bit         | 64 bit    |`clock_time_t` | ?         | 96 bit (7)| 64        | 64 (10)             |
| System Time Unit          | "Tick"        | hw dependent  | µs        | hw dependent  | ?         | µs        | µs        | clock dependent   |
| Portable System Time (9)  | ✗             | ✗             | ✓         | ✗             | ?         | ✓         | ✓         | ✗                 |
| Delays                    | ✓             | ● (1)         | ✓         | ● (4)         | ✓ (5)     | ✓         | ✓         | ✓                 |

✓ = 1st level API support, ● = supported by combining APIs, ✗ = no support

1. Threads can wait until a software timer fires. Software timers accept NULL
   as empty callback. Combined, this can be used to delay execution.
2. Contiki provides with `ctimer_reset()`/`etimer_reset()` which restarts the
   timer from the previous expiration time, allowing to easily achieve
   periodic callbacks.
3. The Rtimer library seems to run the callbacks in IRQ context and the Ctimer
   library in thread context.
4. The Etimer library posts events on timer expiration and a thread can block
   until a specific event has been posted. Combined, this can be used to
   delay execution
5. Only available when using TOSTThreads
6. `xtimer_periodic_wakeup()`/`ztimer_periodic_wakeup()` can be used, but
   requires a dedicated thread allocated for the periodic task
7. Mynewt uses a `struct` with a members `tv_sec` (64 bit) and `tv_usec`
   (32 bit)
8. `ztimer_now()` returns the current time of the given clock. This can be used
   as system timer
9. "Portable System Time" means that two nodes can exchange time stamps over
   network and they still refer to the same point in time, assuming the clocks
   have been synchronized.
10. ztimer optionally keeps each clock's time in 64bit (module ztimer_now64)

# 2. Requirements

The following lists of requirements consist of qualitative and quantitative
requirements. For the quantitative requirements, any improvement will enable
more scenarios to use the timer API. Thus, no meaningful lower bounds can be
specified. Instead, a set of benchmarks will be developed that allow to
continuously improve the performance and to detect any possible regressions of
future changes.

## Feature Requirements

- Software Timers
    - Upper bound for number of timers only given by available resources
    - APIs to set and abort timers
        - Functions for setting timers with both absolute time and relative time
    - Support for periodic timers (support via explicit calls to some
      `timer_reset()` like in Contiki is fine)
    - Long term low resolution software timers that play well with power management
    - Short term high resolution software timers
    - High resolution real time timers
        - Timers that expire at a specific absolute portable time stamp (e.g. a UNIX timestamp)
        - Time synchronization (when enabled and used) must update the timeouts, if needed
        - Justification: E.g. sensor networks for pressure fluctuation monitoring need measurements to be performed synchronized with high accuracy.
        - Note: Real time timers should be optional (so that only applicaitons actually using them pay the overhead in terms of ROM and runtime overhead).
- Delay of program execution
    - API to block calling thread for long periods and allows other threads to run
    - Long term delays must play well with power management
    - Highly precise short term delays (e.g. for bit banging)
- System time
    - APIs to get the current system time in reasonable, hardware independent
      units
        - At least: seconds, milli seconds, micro seconds, nano seconds.
        - Justification: Sensor networks e.g. for monitoring pressure fluctuations require the system time of nodes to be synchronized with high accuracy. E.g. the PTP protocol can provide sub microsecond synchronization accuracy. Thus, there are use cases requiring resolutions below micro seconds and there is hardware providing these resolutions and synchronization accuracy.
    - UNIX epoch as common reference point for high interoperability
    - Support for 64 bit time stamps
        - Justification: Devices running for decades might want to log events
          with milli second resolution timestamps on flash / sd card / ...
        - Note: The rest of the API can be 32 bit, only for time stamps 64 bit
          support is mandatory
    - Optional feature to auto-initialize system time from RTC/RTT during boot
    - APIs to allow implementing network time synchronization
        - The synchronization mechanism should be highly accurate. (E.g. consider the mechanism would read the value from hardware, modify it by adding a time offset determined by some network time synchronization mechanism to it, and store the new value in the hardware: The actually applied offset would be lower by the time that has passed between the read from the hardware and the write to it, reducing accuracy. The actual implementation should be smarter.)
        - Only the system time (the portable 64 bit time stamps) need to be synchronized.
- Utilities for convenience
    - Timer utilities
        - Timer that send a message upon expiration
        - Timer that post an event upon expiration (to allow execution in thread context)
        - Timer that wakes up a thread upon expiration
    - Timeout utilities
        - Timeout for receiving a message
        - Timeout for locking a mutex
    - System time utilities
        - Functions to convert between different time formats, e.g. UNIX and GPS timestamps

## Real-Time Requirements

- The implementation must only disable IRQs for short periods of time. More
  specifically: With *n* software timers used in parallel, the longest period of
  time with interrupts disabled be at most *O(n)*
- The implementation must aim for a minimized number of IRQs caused
- The implementation must aim for keeping its ISRs short; with *n* software
  timers used in parallel, the ISR should run for at most *O(n)*
- The implementation should provide means to execute callbacks in thread
  context

Justification: Real time capable systems require an upper bound for the latency
a high level timer implementation provides. For most application - if not all -
it is trivial to get an upper bound for the number of software timers being
active in parallel. Hence, an *O(f(n))* latency with *n* being the number of
active software timers makes the real time behavior of a system predictable and
easy to estimate. In addition, *f(n)* should increase as slowly as possible,
while not being to strict to rule out reasonable trade-offs e.g. between
ROM/RAM requirements and latency. An *O(n)* latency is e.g. feasible to
implement with a sorted linked list and therefore a reasonable lower bar.

```

## Precision Requirements

- Timer callbacks should never fire prematurely (±1 tick of the underlying
  timer)
- The accuracy of execution delay should be measured with single digit micro
  seconds for short delays in absence of other load on the system
    - It is acceptable if platform specific tweaking is needed to achieve this
    - It is acceptable if minimum hardware requirements are needed to achieve
      this
- Timer callbacks should fire closely after the timer expired
    - In absence of other load on the system, accuracy in the order of single
      digit micro seconds should be achievable when given hardware requirements
      are fulfilled and platform specific tweaking is done

## Performance Requirements

- Efficient use of RAM/ROM
- Optimized for power management
    - The timer implementation should minimize system wake ups
- Efficient use of CPU cycles


## Acknowledgements

Thanks to K. Schleiser, J. Nohlgård, M. Rottleuthner, and all other contributors
to this document and RIOT's timer API.

TODO: Extend as needed.

## References

- [RFC7228 - Terminology for Constrained-Node Networks](https://tools.ietf.org/html/rfc7228)
- TODO: Extend as needed

## Contact

The authors of this memo can be contacted via email at ...
TODO: Add emails of authors prior merging

Additionally, the RIOT developer community can be reached via email at
devel@riot-os.org.
