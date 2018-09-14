# About

This test will measure the amount of messages that could be sent from one
thread to another during an interval of one second. The result amounts to the
number of messages sent, which is half the number of context switches incurred
through sending the messages.

This test application intentionally duplicates code with some similar benchmark
applications in order to be able to compare code sizes.
