Usage of sixlowapp - 6LoWPAN example
====================================
* set up two nodes
    * for nativenet:
        * run `${RIOTBASE}/cpu/native/tapsetup.sh create`
        * run `PORT=tap0 make term`
        * run `PORT=tap1 make term` (from another terminal)
* type `ifconfig` on both nodes
* type `ping <IPv6 address>` on node 1, using one of node 2's IPv6 addresses
* type `nc -l <PORT>` to start a UDP server listening on `<PORT>` on of the nodes
* type `nc <IPv6 address> <PORT> <TEXT>` on the other node to send `<TEXT>` to the first node
