#Cooja and RIOT

This README describes the essential steps you need to take in order to
run RIOT on two networked telosb boards in cooja. The boards will
be simulated by an cycle-accurate hardware simulator called mspsim which
is integrated in cooja.

## Preliminary steps

1. Please download cooja by running the command:

```make cooja```

in the ```pkg``` folder.

2. Make sure you have a recent java JDK installed (including the build tool
```ant``` )


3. Make sure your ```$JAVA_HOME``` variable is set. Please refer to
your operating system documentation for information on how to do this.
As a side-note: on Linux systems it is not sufficient to merely add
this variable to ```/etc/environment```. The variable has to be
exported.

## Compiling the default project for telosb
For the sake of simplicity we will compile and use the ```example/default``` project.

1. Change directory to ```example/default```.

2. Build this application for telosb with the following command:

```make clean all BOARD=telosb```

3. Rename the resulting ```default.elf``` file in ```example/default/bin/telosb/```
to ```default.sky```.

The last step is required since cooja demands that firmware image files (which are
elf files) adhere to the following naming convention: <appname>.<board>.
The sky suffix refers to the Tmote sky platform which is identical to the telosb
platform supported by RIOT.

## Setting up a simulation with cooja
Now, we will start cooja, set up a simple network topology and start a simulation based
on this topology. The default application we built in the previous step will serve
as the firmware for the simulated boards in this network.

1. Change directory to ```pkg/cooja/cooja/tools/cooja```.

2. Depending on your CPU execute either:

```ant run```

or

```ant run64```

3. After cooja started click on ```File``` then ```New simulation```.

4. In the dialog box click on ```Create```.

5. Now a few sub-windows should have opened. Most important are the windows with the title:
```Network```, ```Simulation control``` and ```Mote output```.

6. Now we will add some simulated nodes to the network. Click on ```Motes->Add motes->
Create new mode type->Sky mote```.

7. In the dialog box click on browse and navigate to the previously built and renamed RIOT
default application firmware file (```examples/default/bin/telosb/default.sky```).

8. Now click on ```Create```. In the following dialog box change the field
```Number of new motes``` to 2 and click ```Add motes```.

## Running a simulation / interaction with nodes

To start your simulation click on ```Start``` in the ```Simulation control``` sub-window.
You should now see the output of both nodes (in different colors) in the ```Mote output```
sub-window.

You can open a serial terminal to a node by right-clicking on it in the ```Network``` sub-window
and then choosing the following option: ```Mote tools for Sky...->Mote interface viewer```. In the
interface viewer choose ```Serial port``` from the drop down menu. Now try to enter something like
```ps```. The output should be display in the ```Mote output``` sub-window after a short while.
