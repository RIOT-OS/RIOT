#desvirt
DES-Virt is the [DES-Testbed](http://des-testbed.net/node/4) virtualization framework.

Dependencies
------------
To install all required packages on Debian or Ubuntu, run:

	apt-get install kvm libvirt-bin python-libvirt vlan bridge-utils ebtables

To install all required packages on Fedora or Redhat, run:

	yum install kvm libvirt libvirt-python vconfig bridge ebtables uml_utilities

Bug tracker
-----------
Have a bug or feature request? Please create an issue here on GitHub!

https://github.com/des-testbed/desvirt/issues

HOWTO
=====

setup desvirt
-------------

Run `./setup.py build install` to build and install desvirt on your machine.

In case you want to use desvirt with [RIOT](https://github.com/RIOT-OS/RIOT) nodes: `./topology_creator --node-type riot_native --binary-file ../projects/ccn-lite-client/bin/ccn-lite-client.elf --size 9`


create topology
---------------

To create a topology run `./topology_creator` which creates a 2x2 grid w/o loss `.desvirt/grid2x2.xml` (default).

```
./topology_creator -h
Usage: topology_creator [options]

Options:
  -h, --help            show this help message and exit
  -s TOPO_SIZE, --size=TOPO_SIZE
                        the size of the network
  -t TOPO_TYPE, --type=TOPO_TYPE
                        select type of topology
  -f XMLFILENAME, --file=XMLFILENAME
                        write xml output to FILE
  -d DESCRIPTION, --description=DESCRIPTION
                        a short description of the network
  -l DEF_LOSS_RATE, --loss-rate=DEF_LOSS_RATE
                        set the (default) loss rate for a unicast link in
                        percent
  -g HIGH_LOSS_RATE, --high-loss-rate=HIGH_LOSS_RATE
                        set the loss rate for a diagonal unicast link (only
                        meaningfull for grid topologies) in percent
  -b BCAST_LOSS_RATE, --bcast-loss-rate=BCAST_LOSS_RATE
                        set the loss rate for a broadcast link in percent
  -n NODE_DESC, --node-type=NODE_DESC
                        defines the node type (meshrouter or riot_native)
  -r RADIO_NIC, --radio-interface=RADIO_NIC
                        the name and type of the radio interface (wlan or
                        ieee802154)
  -e BINARY, --binary-file=BINARY
                        the binary file for RIOT native
```

setup topology
--------------

`./vnet --define --name "grid3x3"`


To run a topology run `./vnet --start --name "grid3x3"` which starts the grid.

```
./vnet -h
Usage: vnet [options]

Options:
  -h, --help            show this help message and exit
  -d, --define          set up persistent configuration
  -s, --start           set up virtual network, start all virtual machines
  -q, --stop            stop virtual network, shut down all virtual machines
  -u, --undefine        remove persistent configuration
  -n TOPOLOGY_NAME, --name=TOPOLOGY_NAME
  -l, --list-defined    list all defined networks
  -v, --verbose         increase verbosity
```

