# DESVIRT integration into RIOT

You can find more information about desvirt at
https://github.com/des-testbed/desvirt

## Control DESVIRT via Make

### Variables

This Makefile introduces some additional variables:
 * TOPO (name of the topology to use)
 * TOPO_SIZE (size of the topology to use)
 * TOPO_TYPE (type of the topology to use [line|grid])

### Targets

It defines the following targets:
 * desvirt-topology
 * desvirt-define
 * desvirt-undefine
 * desvirt-list
 * desvirt-start
 * desvirt-stop
 * desvirt-clean
 * desvirt-distclean

#### desvirt-topology

This target creates a new topology file in `RIOTBASE/dist/tools/desvirt/desvirt/.desvirt`
It is necessary to supply the type and size of the new topology with `TOPO_TYPE` and `TOPO_SIZE`.
The make target will create the topology file with the name `$(TOPO_TYPE)$(TOPO_SIZE)`.
Examples:
```
TOPO_TYPE=line TOPO_SIZE=4 make desvirt-topology
TOPO_TYPE=grid TOPO_SIZE=9 make desvirt-topology
```
The names of the files will be: `line4` and `grid9`.

#### desvirt-define

This target defines a new topology. This must be done prior to starting desvirt.
```
TOPO=line4 make desvirt-define
```

#### desvirt-undefine

This target undefines a topology.
```
TOPO=line4 make desvirt-undefine
```

#### desvirt-list

This target lists all defined topologies.
```
make desvirt-list
```

#### desvirt-start

This target starts a new virtualization with the given topology name.
The topology must be defined beforehand.
```
TOPO=line4 make desvirt-start
```

You will get an output like this:
```
cd /home/pschmerzl/RIOT/dist/tools/desvirt/desvirt && ./vnet -s -n line4
vnet           : Loaded statefile .desvirt/lib/line4.macs.
vif            : creating line4_0 for pschmerzl
vif            : Creating tap: line4_0
vif            : creating line4_1 for pschmerzl
vif            : Creating tap: line4_1
vif            : creating line4_2 for pschmerzl
vif            : Creating tap: line4_2
vif            : creating line4_3 for pschmerzl
vif            : Creating tap: line4_3
lossnet        : line4: New link from line4_0 to line4_1, rate=100mbit, loss=0.0, delay=0.0
lossnet        : line4: New link from line4_1 to line4_0, rate=100mbit, loss=0.0, delay=0.0
lossnet        : line4: New link from line4_1 to line4_2, rate=100mbit, loss=0.0, delay=0.0
lossnet        : line4: New link from line4_2 to line4_1, rate=100mbit, loss=0.0, delay=0.0
lossnet        : line4: New link from line4_2 to line4_3, rate=100mbit, loss=0.0, delay=0.0
lossnet        : line4: New link from line4_3 to line4_2, rate=100mbit, loss=0.0, delay=0.0
vnet           : Network Name: line4
vm             : Defining RIOT native process line4_1
riotnative     : Start the RIOT: socat EXEC:'/home/pschmerzl/RIOT/examples/networking/gnrc/networking/bin/native/gnrc_networking.elf line4_1',end-close,stderr,pty TCP-L:4711,reuseaddr,fork
riotnative     : PID: 18235
vm             : Defining RIOT native process line4_0
riotnative     : Start the RIOT: socat EXEC:'/home/pschmerzl/RIOT/examples/networking/gnrc/networking/bin/native/gnrc_networking.elf line4_0',end-close,stderr,pty TCP-L:4712,reuseaddr,fork
riotnative     : PID: 18236
vm             : Defining RIOT native process line4_3
riotnative     : Start the RIOT: socat EXEC:'/home/pschmerzl/RIOT/examples/networking/gnrc/networking/bin/native/gnrc_networking.elf line4_3',end-close,stderr,pty TCP-L:4713,reuseaddr,fork
riotnative     : PID: 18237
vm             : Defining RIOT native process line4_2
riotnative     : Start the RIOT: socat EXEC:'/home/pschmerzl/RIOT/examples/networking/gnrc/networking/bin/native/gnrc_networking.elf line4_2',end-close,stderr,pty TCP-L:4714,reuseaddr,fork
riotnative     : PID: 18238

```

Once the topology is running, you can use `pyterm` to connect to the various
instances, e.g. calling `pyterm -ts 4712` to connect to node `line4_0` from the
example above.

#### desvirt-stop

This target stops a new virtualization with the given topology name.
The topology must be defined beforehand.
```
TOPO=line4 make desvirt-stop
```

#### desvirt-clean

This target resets the desvirt git-folder. All topologies will be deleted.
```
make desvirt-clean
```

#### desvirt-distclean

This target deletes the desvirt folder.
```
make desvirt-distclean
```
