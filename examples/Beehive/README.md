Usage of sixlowapp - 6LoWPAN example
====================================
* set up two nodes
    * for nativenet:
        * run `${RIOTBASE}/cpu/native/tapsetup.sh create`
        * run `PORT=tap0 make term`
        * run `PORT=tap1 make term` (from another terminal)
* type `ifconfig` on both nodes
* type `ping <IPv6 address>` on node 1, using the link local unicast address of node 2 as <IPv6 address` on both nodes
* type `ping <IPv6 address>` on node 1, using the link local unicast address of node 2 as <IPv6 address>
