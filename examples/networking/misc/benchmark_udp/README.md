# UDP Benchmark

This example uses the `benchmark_udp` module to create a stress-test for the RIOT
network stack.

This firmware will act as a client and connect to the benchmark server you can find
in `dist/tools/benchmark_udp`.

## Setup on Hardware

Determine the address of your host machine that will communicate with the RIOT node.
This could be the address of your ethernet interface, or `fd00:dead:beef::1` if you
used the `gnrc_border_router` example and want to run the benchmark on a 6LoWPAN node.

You can either start the benchmark manually by using the `bench_udp start` shell command
or you can configure it to start automatically:

    USEMODULE += auto_init_benchmark_udp
    CFLAGS += -DBENCH_SERVER_DEFAULT=\"fd00:dead:beef::1\"

## Setup on RIOT native

First, make sure you've compiled the application by calling `make`.

Now, create a tap interface:

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

If you only have a single tap device you can just use the broadcast address

    bench_udp start ff02::1

Otherwise use the link-local address of the `tapbr0` interface (if you did set up the tap
devices using `tapsetup`.

## Running the benchmark server

To run the benchmark server on your host machine, follow the instructions found in

    dist/tools/benchmark_udp
