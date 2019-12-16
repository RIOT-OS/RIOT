- RDM: 2
- Title: RIOT's High Level Timer API
- Author: TODO: Add authors prior merging
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
a single alarm.

A high level timer API is needed to provide convenient and portable access to
the hardware features and is essential when multiple virtual timers need to
be provided on top of a single hardware timer.

## Related Timer APIs and Commonly Provided Features

The following list of related APIs provided by other operating systems targeting
constraint devices of class C2 according to RFC 7228 terminology has been
studied:

- FreeRTOS:
    - [Software Timer Feature description](https://www.freertos.org/RTOS-software-timer.html),
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

| Feature                   | FreeRTOS  | Zephyr    | Mbed OS   | Contiki   | TinyOS    | Mynewt    |
|:------------------------- |:--------- |:--------- |:--------- |:--------- |:--------- |:--------- |
| Software Timers           | ✓         | ✓         | ✓         | ✓         | ✓         | ✗         |
| Periodic Software Timers  | ✓         | ✓         | ✓         | ● (2)     | ✓         | ✗         |
| Context of Timer Callback | Thread    | ?         | ?         | Both? (3) | ?         | n.a.      |
| System Time               | ✓         | ✓         | ✓         | ✓         | ✓         | ✓         |
| Delays                    | ✓         | ● (1)     | ✓         | ● (4)     | ✓ (5)     | ✓         |

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
    - Support for periodic timers (support via explicit calls to some
      `timer_reset()` like in Contiki is fine)
    - Long term low resolution software timers that play well with power management
    - Short term high resolution software timers
- Delay of program execution
    - API to block calling thread for long periods and allows other threads to run
    - Long term delays must play well with power management
    - Highly precise short term delays (e.g. for bit banging)
- System time
    - APIs to get the current system time in reasonable units
- Utilities for convenience
    - Timer utilities
        - Timer that send a message upon expiration
        - Timer that post an event upon expiration (to allow execution in thread context)
        - Timer that wakes up a thread upon expiration
    - Timeout utilities
        - Timeout for receiving a message
        - Timeout for locking a mutex

## Real-Time Requirements

- The implementation must only disable IRQs for short periods of time
- The implementation must cause as little IRQs as possible and its ISRs must
  be as short as possible
- The implementation should provide means to execute callbacks in thread
  context

## Precision Requirements

- Timer callbacks should never fire prematurely (+- 1 tick of the underlying
  timer)
- The accuracy of execution delay should be measured with single digit micro
  seconds for short delays
- Timer callbacks should fire as early as possible after the timer expired

## Performance Requirements

- Efficient use of RAM/ROM
- Optimized for power management
    - The timer API should only cause the system to wake up when strictly needed
- Efficient use of CPU cycles

# 3. Design Decisions

TODO: Add these after some discussion.

## Acknowledgements

Thanks to K. Schleiser, J. Nohlgård, M. Rottleuthner, and all other contributors
to this document and RIOT's timer API.

TODO: Extend as needed.

## References

- [RFC7228 - Terminology for Constrained-Node Networks](https://tools.ietf.org/html/rfc7228)
- TODO: Extend as needed

## Revision

TODO: Add revisions, if needed

## Contact

TODO: Do we need this?
