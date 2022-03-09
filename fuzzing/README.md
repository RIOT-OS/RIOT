# Fuzzing

Automated fuzzing tests for RIOT network applications.

## Setup

The following additional dependencies are required:

* [afl][afl homepage]
* [libasan][sanitizers github] (optional but recommended)

## Writing a fuzzing application

The fuzzing setup relies heavily on RIOT's [netapi][netapi doc]
interface, or more generally RIOT's message passing algorithm. RIOT
modules implementing network protocols usually start a single thread
which receives messages via message passing and interprets these as
protocol messages for further processing. This software architecture is
utilized by the fuzzing setup by sending random data to network modules
using message passing.

For each network module which should be fuzzed a protocol specific
fuzzing application needs to be written. This application must
initialize the network module and make sure that the thread is started
and configured correctly. Afterwards, the application must read from
standard input where random data is provided by AFL which must be
converted to a `gnrc_pktsnip_t` by the application. The fuzzing module
provides a utility function (`fuzzing_read_packet`) for this purpose.

Additionally, the fuzzing application is responsible for ensuring the
correct protocol specific encapsulation for the `gnrc_pktsnip_t` type.
For instance, the `gnrc_tcp` module expects each packet, received
via message passing, to be encapsulated in an IP datagram. This
encapsulation is created by the fuzzing application.

Afterwards, the created `gnrc_pktsnip_t` is dispatched to the started
network module thread using `netapi`. As soon as the network module
finished processing the packet, i.e. frees it (or requests the next one
when using the `sock` API), the fuzzing application is terminated and
started again with new random input by AFL. Currently, only a single
packet can be processed per invocation.

## Input Corpus

AFL utilizes mutation-based input generation and as such requires an
initial input corpus. Refer to the [AFL manual][afl-fuzz approach] for
further information an AFL's input generation algorithm. The input
corpus should consist of valid protocol messages and should be as small
as possible. The input corpus for a given fuzzing application is located
in the `input/` subdirectory, each file in this directory represents a
single protocol message. Depending on the specific protocol, an input
corpus may be generated using a script or by extracting packets from a
sniffed sample communication (e.g. using `wireshark`).

When creating a new fuzzing application, it is advisable to test that
packets of the input corpus are successfully processed. This can be done
by setting `ENABLE_DEBUG` for the tested module and running:

	make -C fuzzing/<application> term < fuzzing/<application>/input/<packet>

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
[netapi doc]: https://riot-os.org/api/netapi_8h.html
[afl-fuzz approach]: https://github.com/google/AFL/blob/ca01f9a4c4ccb59d349c729ad3018e339f9aae0c/README.md#2-the-afl-fuzz-approach
