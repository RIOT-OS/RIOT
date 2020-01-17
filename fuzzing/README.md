# Fuzzing

Automated fuzzing tests for RIOT network applications.

## Setup

The following additional dependencies are required:

* [afl][afl homepage]
* [libasan][sanitizers github] (optional but recommended)

## Invocation

Before fuzzing an application it needs to be compiled, to ease detection
of unwanted behaviour (e.g. out-of-bounds buffer accesses), compiling
with `all-asan` is highly recommended. For example:

	make -C fuzzing/<application> all-asan

Afterwards invoke afl using:

	make -C fuzzing/<application> fuzz

### Parallel Fuzzing

Parallel fuzzing is supported through `AFL_FLAGS`, e.g.:

	# Start first AFL instance
	AFL_FLAGS="-M fuzzer01" make -C fuzzing/gnrc_tcp/ fuzz

	# Start second AFL instance in a different terminal
	AFL_FLAGS="-M fuzzer02" make -C fuzzing/gnrc_tcp/ fuzz

[sanitizers github]: https://github.com/google/sanitizers
[afl homepage]: http://lcamtuf.coredump.cx/afl/
