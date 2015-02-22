# About
This script uses desvirt (native port + native net) in order to illustrate
the usage of pytermcontroller.

## Usage
1. Make sure desvirt is setup. In order to do so execute ```make``` in
RIOT/pkg/desvirt. This will also setup a simple network toplogy consisting of
three nodes. Each node will execute RIOT/example/default/bin/native/default.elf.

2. If you haven't already done it compile RIOT/example/default for native:

```Shell
cd your_riot_path/example/default
make
```

3. Execute exampleDesVirt.py. It might be that you are asked for your sudo password. This is required in order
to setup the network bridges etc.

6. You should see an output similar to this:
```
Running preHook
executing: ./vnet --start --name example in: ./../../../../../pkg/desvirt/desvirt
vnet           : Lockfile .desvirt/lib/example.lock exists, network is already defined.
[sudo] password for philipp:
name: example_a1 port: 4711
name: example_a2 port: 4713
name: example_a3 port: 4712
Running experiment
rm -rf /home/philipp/.pyterm/log/*.log
screen -S pyterm-example_a1 -d -m python ./../../pyterm -slocalhost -P 1025 -H example_a1 -rn example_a1 -ts 4711 -ln log
screen -S pyterm-example_a2 -d -m python ./../../pyterm -slocalhost -P 1025 -H example_a2 -rn example_a2 -ts 4713 -ln log
screen -S pyterm-example_a3 -d -m python ./../../pyterm -slocalhost -P 1025 -H example_a3 -rn example_a3 -ts 4712 -ln log
new connection made
dataReceived added: example_a2
new connection made
dataReceived added: example_a1
new connection made
dataReceived added: example_a3
Running postHook
cd /home/philipp/.pyterm/log/..; tar -cjf archived_logs-MyExperiment_2015-02-22_17_05_06.tar.bz2 log/*.log
stop called
screen -X -S pyterm-example_a1 quit
screen -X -S pyterm-example_a2 quit
screen -X -S pyterm-example_a3 quit
vnet           : Loaded statefile .desvirt/lib/example.macs.
vnet           : Loaded statefile .desvirt/lib/example.taps.
vnet           : Loaded statefile .desvirt/lib/example.pids.
vnet           : Network Name: example
vnet           : Shutting down bridge and links...
vnet           : Shutting down nodes...
riotnative     : Kill the RIOT: ../../../examples/default/bin/native/default.elf (28599)
riotnative     : Kill the RIOT: ../../../examples/default/bin/native/default.elf (28602)
riotnative     : Kill the RIOT: ../../../examples/default/bin/native/default.elf (28605)
vnet           : Network stopped.
```

6. The resulting log files can be found in ```~/.pyterm/log```. Each file name in this directory corresponds
to the host name of a node. In addition to that a bz2 compressed tar ball is created when the experiment ends.
This file can be found in ```~/pyterm/``` (the exact filename can be found in the output of script).
