## About
This example application demonstrates the usage of the `Asymcute` MQTT-SN client
library. It provides a number of shell commands that can be used to trigger
selected procedures like connecting to a gateway, registration and subscription
of topics, and publishing of data.

## Setup
For this application to do anything useful, a running MQTT-SN gateway is needed.
If you don't have access to one, RIOT provides a simple way to start one locally
by providing a dedicated `mosquitto_rsmb` make target, type:
```
make mosquitto_rsmb
```

This will download, build, and run the Eclipse Mosquitto.rsmb 'Really Small
Message Broker' [(found here)](https://github.com/eclipse/mosquitto.rsmb).

## Usage
Simply type
```
help
```
for a list of available commands.

## NOTE 1
The UDP socket handling for IPv6 based endpoints in the `Mosquitto.rsmb`
implementation is buggy when it comes to handling link local addresses,
as the implementation does not remember the interface on which data comes in,
hindering it from sending out any responses.

Quick workaround: simply use global addresses

## NOTE 2
It also seems that the `Mosquitto.rsmb` implementation has a bug when it comes
to subscribing to topics: if a topic name was formerly registered and the same
topic name is later used for issuing a subscription request, the gateway will
assign a new topic ID to the same topic name, so publish messages to the
initially assigned topic ID will not be seen by that subscription.
