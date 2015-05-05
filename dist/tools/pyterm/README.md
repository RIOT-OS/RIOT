# About
This directory contains:

* pyterm: A terminal program written in python.

* pytermcontroller: A python module which implements a control server for
pyterms.

* testbeds: Contains classes which abstract certain functions of different
testbeds. The classes are designed to be used in conjuction with
pytermcontroller.

* pytermcontroller-example: This directory contains scripts which
demonstrates some of the features of pytermcontroller. A look into
```pytermcontroller-example/desvirt/exampleDesVirt.py``` is advisable.

# Dependencies

* pyterm: requires a few basic python modules (inlcuding twisted). 
Check the import statements at the top of the ```pyterm``` file for more 
details. 
* pytermcontroller: requires twisted python modules.
* testbeds: most classes require GNU ```screen``` to be in your ```$PATH``` 
The DESTestbed class additionally requires the command ```parallel-ssh```
(see https://code.google.com/p/parallel-ssh/).

# pyterm

```
usage: pyterm [-h] [-p PORT] [-ts TCP_SERIAL] [-b BAUDRATE] [-d DIRECTORY]
              [-c CONFIG] [-s SERVER] [-P TCP_PORT] [-H HOST] [-rn RUN_NAME]
              [-ln LOG_DIR_NAME]

Pyterm - The Python terminal program

optional arguments:
  -h, --help            show this help message and exit
  -p PORT, --port PORT  Specifies the serial port to use, default is
                        /dev/ttyUSB0
  -ts TCP_SERIAL, --tcp-serial TCP_SERIAL
                        Connect to a TCP port instead of a serial port
  -b BAUDRATE, --baudrate BAUDRATE
                        Specifies baudrate for the serial port, default is
                        115200
  -d DIRECTORY, --directory DIRECTORY
                        Specify the Pyterm directory, default is
                        /home/philipp/.pyterm
  -c CONFIG, --config CONFIG
                        Specify the config filename, default is pyterm-
                        isengart.conf
  -s SERVER, --server SERVER
                        Connect via TCP to this server to send output as JSON
  -P TCP_PORT, --tcp_port TCP_PORT
                        Port at the JSON server
  -H HOST, --host HOST  Hostname of this maschine
  -rn RUN_NAME, --run-name RUN_NAME
                        Run name, used for logfile
  -ln LOG_DIR_NAME, --log-dir-name LOG_DIR_NAME
                        Log directory name (default is hostname e.g.
                        /home/foobar/.pyterm/<hostname>)
```


# pytermcontroller

Currently, the module pytermcontroller contains the following classes:

* PubProtocol and PubFactory: For internal use only.
* Experiment: An abstract implementation of an experiment. If you define your own
experiment you are expected to inherent from this class.
* ExperimentRunner: A class which executes a given experiment on a given testbed.
This class also starts a TCP server. A pyterm instance may connect to this server
by using the -s and -P option.

# testbeds

This module contains the following classes:
* Testbed: An abstract class every testbed implementation must inherent from.
It declares multiple functions a testbed should have (e.g. flashing nodes,
initializing nodes, assignment of transceiver addresses etc.)
* DESTestbed: An implementation of Testbed for the DES-testbed at Freie
Universität Berlin (http://des-testbed.net). The init function of this class 
must be supplied with a host file which is a newline separated list of mesh-router 
host names. The numerical transceiver addresses for each node corresponds to 
the line number in the host file. The corresponding ExperimentRuner should 
be executed on on host "uhu.imp.fu-berlin.de", the DES-testbed server. 
* LocalTestbed: This class automatically finds all locally connected boards and
treats those boards as a testbed. This is done by searching for ttyUSB\* devices
in /dev. Transceiver addresses are assigned in the order the devices are found
(typically /dev/ttyUSB0 gets address 1, /dev/ttyUSB1 gets address 2 etc.).
* DesVirtTestbed: Allows you to run your experiment in a network consisting of
native port instances. The init function needs to be supplied with the path to
 desvirt (see https://github.com/des-testbed/desvirt for details) and the name
 of a topology. The topology must be created according to the desvirt howto
 beforehand.
