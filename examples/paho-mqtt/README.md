### About
This application demonstrates the usage of the Eclipse paho MQTT package in RIOT.

### Setup
For using this example, two prerequisites have to be fulfilled:

1. You need a running MQTT broker like Mosquitto broker for example. Take a look at
[Mosquitto Broker](https://mosquitto.org/). Check online any guide that will
help you setting up the broker into some port (a).
For example this one for debian base linux users
[How to setup a Mosquitto MQTT Server and receive data](https://www.digitalocean.com/community/questions/how-to-setup-a-mosquitto-mqtt-server-and-receive-data-from-owntracks).

2. Your RIOT node needs to be able to speak to that broker at the same port you set in 1.

### Setting up RIOT `native` on Linux
- Run `sudo ./dist/tools/tapsetup/tapsetup -c 1`

## Running the example
- Run on RIOT's root directory:

  make -C examples/paho-mqtt all term

- To connect to a broker, use the `con` command:
```
con  <broker ip addr> [port] [clientID] [user] [password] [keepalivetime]
```
  * *broker ip addr*: IPv6 or IPv4 broker address.
  * *port*: broker port. Default 1883
  * *client ID*: is the client id you set up on the broker. Default can be set
     through DEFAULT_MQTT_CLIENT_ID in your makefile. Otherwise is an empty string.
  * *user*: the one set in the broker, check online tutorial to do it regarding chosen broker.
     Default user can be set through DEFAULT_MQTT_USER in your makefile. Otherwise is an empty string.
  * *password*: the one set in the broker, check online tutorial to do it regarding chosen broker.
     Default user can be set through DEFAULT_MQTT_PWD in your makefile. Otherwise is an empty string.
  * *keepalivetime*: keep alive in seconds for your client. Default 10 secs.

- To subscribe to a topic, run `sub` with the topic name as parameter and a QoS
  level between 1 to 3, e.g.
```
sub hello/world 2
```
- To unsubscribe to a topic, run `unsub` with the topic name e.g.
```
unsub hello/world
```

- For publishing, use the `pub` command with a QoS level between 1 to 3:
```
pub hello/world "One more beer, please." 2
```
