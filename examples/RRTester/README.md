# RIOT OS Feedback Scheduling Tester

## Introduction

This project aims to modify the existing scheduling policy in RIOT OS to support a Feedback scheduling policy and to test it by developing a simple application called RRTester.

## Project Goals

1. Modify the RIOT OS scheduler to introduce a Feedback scheduling policy.
2. Implement RRTester application to test the new scheduling policy.

## Feedback Scheduling Policy

- The modified scheduling policy will have 3 queues.
- Quantum for each queue is set to 0.5 seconds.
- Context switches of different threads will be printed on the terminal.

## RRTester Application

RRTester application is deployed as a RIOT application. It spawns 5 new threads with different priorities and service times as follows:

| Thread Name | Priority                | Service Time (s) |
|-------------|-------------------------|------------------|
| Main        | THREAD_PRIORITY_MAIN    | -                |
| A           | THREAD_PRIORITY_MAIN + 1| 3                |
| B           | THREAD_PRIORITY_MAIN + 2| 6                |
| C           | THREAD_PRIORITY_MAIN + 3| 4                |
| D           | THREAD_PRIORITY_MAIN + 4| 5                |
| E           | THREAD_PRIORITY_MAIN + 5| 2                |

Each thread includes a `printf()` function to print the currently running thread name and the remaining execution time in seconds.

### Prerequisites

- Install RIOT OS on your development environment.

### Installation

1. Clone this repository.
2. Build and flash the RRTester application to your RIOT device.
3. Start the terminal to view the output.

## Usage

- After flashing the RRTester application, observe the terminal to view the thread execution and context switches.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).


