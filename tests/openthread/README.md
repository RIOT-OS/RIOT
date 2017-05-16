## OpenThread on RIOT

This test demonstrates how to use NEST's OpenThread stack from [OpenThread](https://github.com/openthread/openthread) on
RIOT. The [Command Line Interface](https://github.com/openthread/openthread/blob/master/examples/apps/cli/README.md) of
OpenThread was ported. Please check the [full
documentation](https://github.com/openthread/openthread/blob/master/src/cli/README.md) of the CLI for usage information.

## Quick usage

To test OpenThread in RIOT, you can do the following:

1. Flash nodes with `make BOARD=<target> clean all flash`
2. Write `start` on one node.
3. Check the state of the node with `state`. In the beggining should be `detached`, but after some seconds it should
   become `leader`
4. Write `start` on another node. The second node should become `router` if there's a leader.
5. Get the mesh IP address of a node with `ipaddr`.
   `ipaddr`
   `fdde:ad00:beef::ff:fe00:8000`
   `fe80::ff:fe00:8000`
   `fdde:ad00:beef:0:946a:c722:a5d9:8481`
   `fe80::3984:f4eb:d182:5dae`
6. Ping from another node with `ping fdde:ad00:beef:0:946a:c722:a5d9:848`.
7. You cna try IEEE802.15.4 scan with `scan` command
8. You can also check other commands with `help`

## OpenThread status

OpenThread is still under development. In case of any bug, please report via GitHub issue.


## Known issues

* When the network traffic is really high, sometimes some nodes crash silently. 
* In some cases there are isolated leaders that are not able to recover to the main partition. These won't affect the
  rest of the network, but this node becomes unreachable.
