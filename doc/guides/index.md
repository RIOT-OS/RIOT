---
title: RIOT in a nutshell
description: Welcome to the RIOT documentation
---

![Riot Logo](img/riot-logo-large.png "RIOT Logo")

### Welcome to the friendly Operating System for the Internet of Things.

RIOT is an open-source microcontroller operating system, designed to match
the requirements of Internet of Things (IoT) devices and other embedded
devices. These requirements include a very low memory footprint (on the order
of a few kilobytes), high energy efficiency, real-time capabilities, support
for a wide range of low-power hardware, communication stacks for wireless and
communication stacks for wired networks.

RIOT provides threading, multiple network stacks, and utilities which
include cryptographic libraries, data structures (bloom filters, hash tables,
priority queues), a shell and more. RIOT supports a wide range of
microcontroller architectures, radio drivers, sensors, and configurations for
entire platforms, e.g. Atmel SAM R21 Xplained Pro, Zolertia Z1, STM32 Discovery
Boards etc. (see the list of
[supported boards](https://www.riot-os.org/boards.html)).
Across all supported hardware (32-bit, 16-bit, and 8-bit platforms), RIOT
provides a consistent API and enables C and C++ application programming,
with multithreading, IPC, system timers, mutexes etc.

A good high-level overview can be found in the article
[RIOT: An Open Source Operating System for Low-End Embedded Devices in
the IoT](https://www.riot-os.org/assets/pdfs/riot-ieeeiotjournal-2018.pdf)
(IEEE Internet of Things Journal, December 2018).

## Contribute to RIOT

RIOT is developed by an open community that anyone is welcome to join:

- Download and contribute your code on
  [GitHub](https://github.com/RIOT-OS/RIOT). You can read about how to
  contribute [in our contributing
  document](https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md).
- Sign-up to our [forum](https://forum.riot-os.org/) to ask for help using RIOT
  or writing an application for RIOT, discuss kernel and network stack
  development as well as hardware support, or to show-case your latest project.
- Follow us on [Mastodon][mastodon-link] for news from the RIOT
  community.
- Regarding critical vulnerabilities we would appreciate if you give us a
  90-days head-start by reporting to security@riot-os.org, before making your
  information publicly available
- Contact us on Matrix for live support and discussions:
  [riot-os:matrix.org](https://matrix.to/#/#riot-os:matrix.org)

[mastodon-link]: https://floss.social/@RIOT_OS
