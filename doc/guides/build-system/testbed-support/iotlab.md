---
title: Using the IoT-LAB testbed
description: Learn how to control IoT-LAB testbed nodes via the RIOT buildsystem
---

The FIT IoT-LAB lab is a very-large-scale testbed of IoT devices, located for the most part in
France.
IoT-LAB provides some excellent guides how to use the testbed with RIOT on its own pages.
Check, e.g., its [Wiki][IoT-LAB Wiki] or the
["Learn" section on its website](https://www.iot-lab.info/learn/).
This guide provides insights how to use the IoT-LAB with our `make` build system.

## Control IoT-LAB experiments via make

### Requirements

This feature requires you to have a valid account for the FIT IoT-LAB
([registration](https://www.iot-lab.info/testbed/signup) there is open for everyone) and the
[`iotlabcli` tools](https://pypi.org/project/iotlabcli/) to be installed.

### Description

The folder `dist/testbed-support/` contains a `Makefile.iotlab` that defines
targets to control RIOT experiments on IoT-LAB using the GNU Make build
system. In order to use this, one has to include this Makefile at the end of
the application's Makefile, like this:
```
include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab
```

For [boards supported by both the IoT-LAB and RIOT][IoT-LAB Wiki], i.e. unless you are porting a new
board, **this is already done automatically**.

### Variables

This Makefile introduces some additional variables (default values in
brackets):
 * `IOTLAB_NODES` (5)
 * `IOTLAB_DURATION` (30 minutes)
 * `IOTLAB_SITE` (grenoble.iot-lab.info)
 * `IOTLAB_TYPE` (m3:at86rf231)
 * `IOTLAB_AUTH` ($HOME/.iotlabrc)
 * `IOTLAB_USER` (taken from $IOTLAB_AUTH)
 * `IOTLAB_EXP_ID` (taken from first experiment in running state)
 * `IOTLAB_EXP_NAME` (RIOT_EXP)
 * `IOTLAB_PHY_NODES`
 * `IOTLAB_PROFILE`
 * `IOTLAB_EXCLUDE_NODES`
 * `IOTLAB_LOGGING`
 * `IOTLAB_TMUX`

### Format of a Resource ID
Both variables `IOTLAB_PHY_NODES` and `IOTLAB_EXCLUDE_NODES` use the resource id
string format as specified in the output of `iotlab-experiment submit --help`.
An example would be: `1-3+7+10-13`

### Targets

It defines the following targets:
 * `iotlab-exp`
 * `iotlab-flash`
 * `iotlab-reset`
 * `iotlab-term`

**Please note:** All targets that require an already running experiment will
use the first experiment of the user that has already entered state "Running"
if `IOTLAB_EXP_ID` is not set.

#### iotlab-exp

This schedules a new experiment on the FIT IoT-LAB and waits until it enters
"Running" state. It will request `IOTLAB_NODES` nodes of type `IOTLAB_TYPE`
for `IOTLAB_DURATION` minutes at site `IOTLAB_SITE` using `IOTLAB_PROFILE`
(if provided). With `IOTLAB_PHY_NODES` it is possible to choose specific nodes
for this experiment by using the resource id string format as described above.
Note that the usage of `IOTLAB_PHY_NODES` ignores `IOTLAB_NODES`. It will also flash the
binary of the current application to all registered nodes. The name of the
experiment is set to "`RIOT_EXP`" or "`RIOT_EXP_$(IOTLAB_EXP_NAME)`"
if `IOTLAB_EXP_NAME` is defined.
If `IOTLAB_LOGGING` is set to `1`, then the output of all nodes will be logged on the server
into a file called "`RIOT_LOG-<EXPNAME>-<EXPID>`".

#### iotlab-flash

This target updates the application on all registered nodes of the given
experiment to the current version of the application.
Certain nodes can be excluded by listing them in the `IOTLAB_EXCLUDE_NODES` variable
using the resource id string format as described above.

#### iotlab-reset

This target resets all registered nodes of the given experiment.
Certain nodes can be excluded by listing them in the `IOTLAB_EXCLUDE_NODES` variable
using the resource id string format as described above.

#### iotlab-term

Uses ssh to login the user on the IoT-LAB server of the specified site and
start the `serial_aggregator` to communication with all registered nodes.
If `IOTLAB_LOGGING` is set to `1`, then closing the connection with `CTRL+C/D` will also quit
the logging process. If you enabled TMUX with `IOTLAB_TMUX`, detach from the
server-side tmux process with `CTRL+A-D` (or as defined in your server-side
`.tmux.conf` file)

## Use a Single Node Like a Local One

Using the extension `dist/testbed-support/makefile.iotlab.single.inc.mk` (also included
automatically for IoT-LAB supported boards), a single node can be controlled as if it were local,
i.e., you can interact with it with the normal `flash`, `term`, `debug`, etc. `make` targets. For
this you need to provide some environment variables:

- `IOTLAB_NODE` is required. If you only have one experiment running and only node of that board
  type in that experiment, you can set it to `auto`, e.g.,

  ```sh
  BOARD=iotlab-m3 IOTLAB_NODE=auto make -C examples/basic/hello-world flash
  ```

  You can also use the FQDN of the node if you have multiple nodes in your experiment.

  ```sh
  BOARD=iotlab-m3 IOTLAB_NODE=m3-42.grenoble.iot-lab.info \
      make -C examples/basic/hello-world flash
  ```

  If you use `IOTLAB_NODE=auto`, the `make info-iotlab-node` command provides you with the FQDN of
  the node that was selected.

- `IOTLAB_EXP_ID` is only required if you have more than one experiment running, similar as for the
  `iotlab-*` targets above. Running `make` without it will warn you about the ambiguity.

The semantics of all other environment variables for the `iotlab-*` targets stay mostly the same,
the ones involved in starting an experiment are ignored though.

[IoT-LAB Wiki]: https://github.com/iot-lab/iot-lab/wiki/Riot-support
