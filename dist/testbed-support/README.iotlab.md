# RIOT integration into IoT-LAB

Check the Wiki to see how to build and run RIOT on FIT IoT-LAB:
https://github.com/iot-lab/iot-lab/wiki/Riot-support

## Control IoT-LAB via Make

### Requirements

This feature requires to have a valid account for the FIT IoT-LAB
(registration there is open for everyone) and the
[iot-lab/cli-tools](https://github.com/iot-lab/cli-tools) to be installed.

### Description

The folder `dist/testbed-support/` contains a `Makefile.iotlab` that defines
some targets to control RIOT experiments on IoT-LAB using the GNU Make build
system. In order to use this, one has to include this Makefile at the end of
the application's Makefile, like this:
```
include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab
```
### Variables

This Makefile introduces some additional variables (default values in
brackets):
 * IOTLAB_NODES (5)
 * IOTLAB_DURATION (30 minutes)
 * IOTLAB_SITE (grenoble.iot-lab.info)
 * IOTLAB_TYPE (m3:at86rf231)
 * IOTLAB_AUTH ($HOME/.iotlabrc)
 * IOTLAB_USER (taken from $IOTLAB_AUTH)
 * IOTLAB_EXP_ID (taken from first experiment in running state)
 * IOTLAB_EXP_NAME (RIOT_EXP)
 * IOTLAB_PHY_NODES

### Targets

It defines the following targets:
 * iotlab-exp
 * iotlab-flash
 * iotlab-reset
 * iotlab-term

**Please note:** All targets that require an already running experiment will
use the first experiment of the user that has already entered state "Running"
if `IOTLAB_EXP_ID` is not set.

#### iotlab-exp

This schedules a new experiment on the FIT IoT-LAB and waits until it enters
"Running" state. It will request `IOTLAB_NODES` nodes of type `IOTLAB_TYPE`
for `IOTLAB_DURATION` minutes at site `IOTLAB_SITE`. With `IOTLAB_PHY_NODES`
it is possible to choose specific nodes for this experiment by using the resourceid
string format defined in `experiment-cli submit --help` (example: 1-3+7+10-13).
Note that the usage of `IOTLAB_PHY_NODES` ignores `IOTLAB_NODES`. It will also flash the
binary of the current application to all registered nodes. The name of the
experiment is set to "RIOT_EXP" or "RIOT_EXP_$(IOTLAB_EXP_NAME)"
if `IOTLAB_EXP_NAME` is defined.

#### iotlab-flash

This target updates the application on all registered nodes of the given
experiment to the current version of the application.

#### iotlab-reset

This target resets all registered nodes of the given experiment.

#### iotlab-term

Uses ssh to login the user on the IoT-LAB server of the specified site and
start the `serial_aggregator` to communication with all registered nodes.
