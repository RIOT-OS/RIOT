# stdio_notify test application

## Introduction

This application tests and demonstrates the functionality of the
`stdio_notify` module, which can be used to invoke a callback whenever data is
available on the stdin stream. The application has a single thread that handles
two tasks:

- Echo any data received on stdin back to stdout.
- Print a message to stdout every five seconds.

The main loop unblocks if any of the two tasks needs to be executed.

## Expected result

Data that is sent to stdin is echoed back to stdout, while a message is printed
to stdout every five seconds.
