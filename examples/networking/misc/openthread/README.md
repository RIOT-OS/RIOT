## OpenThread on RIOT

This example demonstrates how to use the [OpenThread](https://github.com/openthread/openthread)
open source implementation of [Thread](https://threadgroup.org/) on RIOT.

The [Command Line Interface](https://github.com/openthread/openthread/blob/master/examples/apps/cli/README.md) of
OpenThread was ported. Please check the
[full documentation](https://github.com/openthread/openthread/blob/master/src/cli/README.md)
of the CLI for usage information. Commands start with prefix 'ot'.

You can either build a FTD or MTD firmware:
- MTD: A Minimal Thread Device does not have router functionality compiled in.
       An MTD may or may not be sleepy.
- FTD: A Full Thread Device has router functionality compiled in.

## Quick usage

With RIOT port, a node is auto-setup and ready to communicate with
this configuration:
```
OPENTHREAD_PANID=0xbeef
OPENTHREAD_CHANNEL=26
```

You can pass the panid/channel independently when building the firmware:
```
make BOARD=<target> OPENTHREAD_PANID=0xaaaa OPENTHREAD_TYPE=ftd flash term
```
```
make BOARD=<target> OPENTHREAD_CHANNEL=20 OPENTHREAD_TYPE=ftd flash term
```

To try OpenThread in RIOT, you can do the following:

1. Flash nodes with MTD or FTD functionality:
```
make BOARD=<target> clean all flash OPENTHREAD_TYPE=mtd
```
```
make BOARD=<target> clean all flash OPENTHREAD_TYPE=ftd
```

2. Check the state of the node with `ot state`. In the beginning, it should be
`detached`, but after some seconds it should become `leader`

3. Start another node and check that it becomes `router`. There is only one
leader in a Thread network.

4. Get the mesh IP address of a node with `ot ipaddr`.
```
ipaddr
   fdde:ad00:beef::ff:fe00:8000
   fe80::ff:fe00:8000
   fdde:ad00:beef:0:946a:c722:a5d9:8481
   fe80::3984:f4eb:d182:5dae
```
5. Ping from another node with
```
ping fdde:ad00:beef:0:946a:c722:a5d9:848
```

6. You can try IEEE802.15.4 scan with `ot scan` command

7. You can also check other commands with `ot help`


## OpenThread port on RIOT status

OpenThread port on RIOT is stable. In case of any bug, please report via
[GitHub issue](https://github.com/RIOT-OS/RIOT/issues/new?template=bug_report.md&title=Bug).
