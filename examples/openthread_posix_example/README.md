# openthread_posix_example

This example shows the `posix_sockets` example running with OpenThread as network stack.  The usage is exactly the same
as the `posix_sockets` example, but includes 2 extra commands for obtaining node ip address and Thread status.


## About Thread stack
You can find more information about Thread networks in
http://threadgroup.org/Portals/0/documents/whitepapers/Thread%20Stack%20Fundamentals_v2_public.pdf

## The shell commands

* `ot_state` - shows the Thread state of a node (leader, router, child)
* `ot_ipaddr` - shows the IP addresses associated to a node (link local, mesh, etc).

## Example setup
An example usage of this application could be the following setup (tested in IoT-LAB):

1. Flash nodes with `make BOARD=<target> clean all flash`
2. Wait some seconds.
3. Write command `ot_state` in each node. If everything went OK, only one node should be `leader`
   and the reset should be either `router` or `child`
4. Read the ip address of a single node with `ot_ipaddr`:
   `ot_ipaddr`
   `fdde:ad00:beef::ff:fe00:8000`
   `fe80::ff:fe00:8000`
   `fdde:ad00:beef:0:946a:c722:a5d9:8481`
   `fe80::3984:f4eb:d182:5dae`
   The third address (fdde:ad00:beef:0:946a:c722:a5d9:8481) is the mesh address.
5. Start posix udp server in port 80:
   `udp server start fdde:ad00:beef:0:946a:c722:a5d9:8481 80`
6. Send UDP messages from another node:
   `udp send fdde:ad00:beef:0:946a:c722:a5d9:8481 80 RIOT`
   `Success: send 4 byte to fdde:ad00:beef:0:946a:c722:a5d9:8481:80`
7. You should get a `Received data: RIOT` message in the receiving node.
   `1470061384.130128;m3-6;> Received data: RIOT`
8. Enjoy! You can try to setup a big network to test multi-hop features of the Thread network.
9. Also, you can try disabling some nodes. The network should automatically heal itself (add a new leader, replace
   children by routers, etc).


## Port status
* Only `conn_udp` supported for the moment (no `conn_ip` yet)
* `ifconfig` not supported yet (use `ot_ipaddr` for ip addresses)
* OpenThread and this port are still under development. If you find any bug please report it via Github issues.
* Only tested with at86rf2xx radios.

## Known issues

* When the network traffic is really high, sometimes some nodes crash silently. 
* In some cases there are isolated leaders that are not able to recover to the main partition. These won't affect the
  rest of the network, but this node becomes unreachable.

## Future work

* Add support for Thread Border Router via NEST's Wpantund (https://github.com/openthread/wpantund)
* Adapt to new conn API.
