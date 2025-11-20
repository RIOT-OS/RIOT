## Decawave TWR_ALOHA Example

This example allows testing different two-way ranging algorithms between
two boards supporting a dw1000 device. This makes use of the uwb-core
pkg.


#### IoT-LAB Setup

IoT-LAB can be used to test the application.

##### Prerequisites

1. Install iotlab-cli tools

    - `pip install iotlabcli`

2. Create an iotlab account

    - https://www.iot-lab.info/testbed/signup

3. Authenticate:

```shell
$ iotlab-auth --user <username> --password <password>
```

##### Launch an Experiment

1. Submit a 60 minutes experiment with 2 dwm1001 boards:

```shell
$ iotlab-experiment submit -n "twr-aloha" -d 60 -l 2,archi=dwm1001:dw1000+site=saclay
```

2. Wait for the experiment to be in the Running state:

```shell
$ iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later.

3. Get the experiment nodes list:

```shell
$ iotlab-experiment  --jmespath="items[*].network_address | sort(@)" get --nodes
[
    "dwm1001-1.saclay.iot-lab.info",
    "dwm1001-10.saclay.iot-lab.info"
]
```

When flashing the devices set `IOTLAB_NODE` to one of the above values, e.g. for
the first node: `IOTLAB_NODE=dwm1001-2.saclay.iot-lab.info`.

##### Free up the resources

You are done with the experiment, so stop your experiment to free up the
devices:

```shell
$ iotlab-experiment stop
```

### Setup

1. In two separate terminals flash two nodes and open a serial connection to the
board, prefix with IOTLAB_NODE if using IoT-LAB

```shell
$ make -C examples/twr_aloha/ flash term -j
$ make -C examples/twr_aloha/ flash term -j
```

1. On at least one node enable listening and get its short address

```shell
> twr lst on
[twr]: start listening
> ifconfig
Iface  3        HWaddr: C8:0C  NID: DE:CA

                Long HWaddr: 08:2B:31:07:CC:52:C8:0C
```

1. On the second node send a request to neighbors short address:

```shell
>twr req C8:0C
[twr]: start ranging
```

Both nodes should show the ranging result:

```shell
> {"t": 55158, "src": "02:A1", "dst": "C8:0C", "d_cm": 39}
```

By default `twr-ss` (Single-Sided Two-Way-Ranging) is used, try using different
protocols, check the ones available with `help` command:

```shell
> twr req --help
Usage:
        twr req <short_addr> [-p <proto>] [-c <count>] [-h][-i <ms interval>]
        - short_addr: short address of request destination
        - count: number of requests (default: 1)
        - ms interval: wait interval milliseconds between requests(default: 1000)
        - proto: twr protocol (ss|ss-ext|ss-ack|ds|ds-ext, default: ss)
        twr lst on: start listening for ranging requests
        twr lst off: stop listening for ranging requests
```

1. Perform range requests with different intervals, protocols etc...

```shell
> twr req C8:0C -c 5 -i 10
[twr]: start ranging
{"t": 251588, "src": "02:A1", "dst": "C8:0C", "d_cm": 38}
{"t": 251600, "src": "02:A1", "dst": "C8:0C", "d_cm": 37}
{"t": 251610, "src": "02:A1", "dst": "C8:0C", "d_cm": 38}
{"t": 251620, "src": "02:A1", "dst": "C8:0C", "d_cm": 37}
```

```shell
> twr req C8:0C -c 5 -i 10 -p ds
[twr]: start ranging
{"t": 312513, "src": "C8:0C", "dst": "02:A1", "d_cm": 36}
{"t": 312525, "src": "C8:0C", "dst": "02:A1", "d_cm": 37}
{"t": 312535, "src": "C8:0C", "dst": "02:A1", "d_cm": 35}
{"t": 312545, "src": "C8:0C", "dst": "02:A1", "d_cm": 35}
```

### Automatic Test

Pre-Requisites:

- `pip install rioctrl pyserial`

A basic automatic test for the provided shell is included, run with:

```shell
$ make -C examples/twr_aloha flash test-with-config
```

The application provides a `ShellInteraction` that can be used for
scripting UWB experiments or testing.
