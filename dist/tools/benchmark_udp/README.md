# UDP Benchmark server

This is a simple tool to generate load and evaluate the performance and reliability of a network.
Clients will periodically send UDP packets to the benchmark server, the server keeps track of
how many packets have been received and how many the nodes have reported to send.

By default the server will also echo the packets back to the sender so round-trip time can be
measured.

### Usage

### Server

Run the binary you find in `bin/benchmark_server`.

e.g. to listen on all addresses on port 12345 run

    bin/benchmark_server :: 12345

There are a few command line options available:

 - `-i <interval>` to control the send interval in µs
 - `-s <size>` to control the test packet payload
 - `-o` for one-way mode where only the clients send packets to the server, but the server doesn't echo them back.

Output:

 - 'host': client address
 - 'bandwidth': average bandwidth since the last configuration package
 - 'num TX': number of packaged produced by the client since the configuration package
 - 'num RX': number of packaged received by the server since the configuration package
 - 'num RT': number of server echos received by the client since the last configuration package
 - 'RTT':      round trip time client->server->client (last package received by client)

### Client

On the application that you want to benchmark, add the `benchmark_udp` module.

If you have the shell enabled you can then start the benchmark manually by

    bench_udp start <address> <port>

If port is omitted it will default to `12345` (`BENCH_PORT_DEFAULT`), if the address is omitted `fd00:dead:beef::1` (`BENCH_SERVER_DEFAULT`) will be used.

If the benchmark should be started automatically, add the `auto_init_benchmark_udp` module.
In this case, `BENCH_SERVER_DEFAULT` and `BENCH_PORT_DEFAULT` will be used.

They can be overwritten via CFLAGS:

    CFLAGS += -DBENCH_SERVER_DEFAULT=\"<addr>\"
    CFLAGS += -DBENCH_PORT_DEFAULT=<port>
