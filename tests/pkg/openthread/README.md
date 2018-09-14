## OpenThread on RIOT

This test demonstrates the [OpenThread](https://github.com/openthread/openthread) stack running on RIOT. When flashed,
it will initialize the OpenThread Command Line Interface for interacting with the stack.

## Quick usage

To test OpenThread on RIOT, you can do the following:

1. Flash nodes with `make BOARD=<target> clean all flash`
2. Write `panid 0x1234`, `ifconfig up` then `thread start` on one node.
3. Check the state of the node with `state`. In the beggining should be `detached`, but after some seconds it should
   become `leader`
4. Write `panid 0x1234`, `ifconfig up` then `thread start` on another node.
The second node should become `child` or `router` if there's a leader.
5. Get the mesh IP address of a node with `ipaddr`.

```
    ipaddr
   fdde:ad00:beef::ff:fe00:8000
   fe80::ff:fe00:8000
   fdde:ad00:beef:0:946a:c722:a5d9:8481
   fe80::3984:f4eb:d182:5dae
```

   Addresses starting with `fd` are mesh-local, and addresses starting with `fe80` are link-local.
   Mesh-local address types that contain `ff:fe00` are classified as Router Locator (RLOC). Mesh-local address types
   that don't contain `ff:fe00` are Endpoint Identifies (EID).
6. Ping from another node to a mesh-local address with `ping fdde:ad00:beef:0:946a:c722:a5d9:8481`.
7. You can try IEEE802.15.4 scan with `scan` command
8. You can also check other commands with `help`
9. Enjoy!

## Note

See the [OpenThread CLI Reference](https://github.com/openthread/openthread/blob/master/src/cli/README.md) for more information about OpenThread CLI commands
