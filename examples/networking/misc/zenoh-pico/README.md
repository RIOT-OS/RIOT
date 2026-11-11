# zenoh-pico

[zenoh](https://zenoh.io) is a protocol for publications, subscriptions and
queries over key expressions, e.g. `demo/riot/value`, or `demo/**` for all
the keys under `demo`. This example uses
[zenoh-pico](https://github.com/eclipse-zenoh/zenoh-pico), the implementation
of zenoh for constrained devices (`zenoh-pico` package), over TCP and the IPv6
stack of RIOT (GNRC).

The `zenoh` shell command opens a zenoh session, either in peer mode between
two nodes, without a router (one node listens, the other one connects to
it), or in client mode, connected to a zenoh router (`zenohd`), e.g. on a
Linux host reached through a border router:

```
> zenoh
usage: zenoh listen <locator>     peer session, listening, e.g. tcp/[::]:7447
       zenoh connect <locator>    peer session, connected to a listening peer
       zenoh client <locator>     client session, connected to a zenoh router
       zenoh pub <key> <value>    publish a value
       zenoh sub <key>            subscribe, print the samples
       zenoh queryable <key>      answer the queries with the board name
       zenoh get <key>            query, print the replies
       zenoh close                close the session
```

A locator is `tcp/[<IPv6 address>]:<port>`, with the interface for a link
local address, e.g. `tcp/[fe80::e406:76f:d97e:c60%6]:7447` for interface 6.

## Two nodes

Flash two nodes with an IEEE 802.15.4 radio, or start two native instances
(see below):

```
make BOARD=nrf52840dk flash term
```

On the first node, get its link local address and its interface with
`ifconfig`, then listen, subscribe to the keys under `demo` and declare a
queryable:

```
> ifconfig
Iface  6  HWaddr: 32:22  Channel: 26  NID: 0x23  PHY: O-QPSK
          ...
          inet6 addr: fe80::e406:76f:d97e:c60  scope: link  VAL
> zenoh listen tcp/[::]:7447
zenoh: peer session listening on tcp/[::]:7447
> zenoh sub demo/**
zenoh: subscribed to demo/**
> zenoh queryable demo/board
zenoh: queryable demo/board
```

On the second node, connect to the first one, publish and query:

```
> zenoh connect tcp/[fe80::e406:76f:d97e:c60%6]:7447
zenoh: peer session connected to tcp/[fe80::e406:76f:d97e:c60%6]:7447
> zenoh pub demo/hello world
zenoh: published world on demo/hello
> zenoh get demo/**
> zenoh: reply demo/board: native64
zenoh: end of the replies
```

The first node prints the sample:

```
> zenoh: demo/hello: world
```

Both nodes publish, subscribe and query in the same way.

Close the connecting node first (`zenoh close`), then the listening one. The
listening node keeps the connection until the other node closes its session:
closed first, it waits for the end of the TCP connection, up to the
connection timeout of GNRC (`CONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS`,
2 minutes).

## Native

Two native instances communicate over two tap interfaces bridged together,
see `dist/tools/tapsetup`:

```
sudo dist/tools/tapsetup/tapsetup --create 2
make BOARD=native64 PORT=tap0 all term
make BOARD=native64 PORT=tap1 all term
```

or over a simulated IEEE 802.15.4 network with `USE_ZEP=1`, through the ZEP
dispatcher of RIOT:

```
make -C dist/tools/zep_dispatch
dist/tools/zep_dispatch/bin/zep_dispatch ::1 17754
make BOARD=native64 USE_ZEP=1 all term
```

## A zenoh router on a Linux host

The nodes reach a Linux host through a border router, e.g. the
[gnrc_border_router](../../gnrc/border_router/README.md) example on a board
with an IEEE 802.15.4 radio, connected to the host: with `make term`, its
`start_network.sh` script gives the prefix `2001:db8::/64` to the nodes and
the host has the address `fd00:dead:beef::1`. For example, do the following
to run a border router on an nrf52840dk board:

```
make -C ../../gnrc/border_router flash term BOARD=nrf52840dk
```

Download and run the zenoh router on the host, the same version as zenoh-pico (1.10.2),
e.g. `zenohd` of the standalone release of zenoh, with its REST plugin:

```
wget -q -N https://github.com/eclipse-zenoh/zenoh/releases/download/1.10.2/zenoh-1.10.2-x86_64-unknown-linux-gnu-standalone.zip
unzip -o -d zenoh zenoh-1.10.2-x86_64-unknown-linux-gnu-standalone.zip
cd zenoh
./zenohd -l 'tcp/[::]:7447' --no-multicast-scouting --rest-http-port 8000
```

Flash the zenoh example application on a second nrf52840dk that is used as the zenoh
node:

```
make flash term BOARD=nrf52840dk PORT=<TTY port of the zenoh node>
```

On the zenoh node, open a client session to the router, subscribe and declare a
queryable:

```
> zenoh client tcp/[fd00:dead:beef::1]:7447
zenoh: client session connected to tcp/[fd00:dead:beef::1]:7447
> zenoh sub demo/**
> zenoh queryable demo/board
```

On the host, publish, query and subscribe with the REST plugin. The
publication is printed by the subscriber of the node (`zenoh: demo/hello:
world`), the query gets the reply of its queryable:

```
curl -X PUT -d world http://localhost:8000/demo/hello
curl http://localhost:8000/demo/board
[{"key":"demo/board","value":"iotlab-m3","encoding":"text/plain","timestamp":null}]
```

The subscription prints the publications of the nodes, e.g. `zenoh pub demo/value 42`:

```
curl -N -H 'Accept: text/event-stream' 'http://localhost:8000/demo/**'
event: PUT
data: {"key":"demo/value","value":"42","encoding":"text/plain","timestamp":"..."}
```
