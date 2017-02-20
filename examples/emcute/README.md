## About
This application demonstrates the usage of the emCute (MQTT-SN) module in RIOT.

## Setup
For using this example, two prerequisites have to be fullfilled:

1. You need a running MQTT broker that supports MQTT-SN or a running MQTT-SN
   gateway that is connected to a running MQTT broker
2. Your RIOT node needs to be able to speak to that broker/gateway


### Setting up a broker
In general, any MQTT-SN capable broker or broker/gateway setup will do.
Following a quick instruction on how-to setup the Mosquitto Real Simple Message
Broker:

1. Get the RSMB here: https://github.com/eclipse/mosquitto.rsmb:
```
git clone https://github.com/eclipse/mosquitto.rsmb.git
```

2. Go into the source folder and build the RSMB
```
cd mosquitto.rsmb/rsmb/src
make
```

3. Create a config file. In this case we run the RSMB as MQTT and MQTT-SN
   capable broker, using port 1885 for MQTT-SN and 1886 for MQTT and enabling
   IPv6, so save the following to `config.conf`:
```
# add some debug output
trace_output protocol

# listen for MQTT-SN traffic on UDP port 1885
listener 1885 INADDR_ANY mqtts
  ipv6 true

# listen to MQTT connections on tcp port 1886
listener 1886 INADDR_ANY
  ipv6 true
```

4. Start the broker:
```
./broker_mqtts config.conf
```

You can refer to
https://rawgit.com/MichalFoksa/rsmb/master/rsmb/doc/gettingstarted.htm for more
configuration options.


### Setting up RIOT `native`
When running this example under native, we must configure some global addresses,
as the RSMB doesn't seems to be able to handle link-local addresses. So for a
single RIOT native instance, we can do the following:

1. Setup `tap` and `tabbr` devices using RIOT's `tapsetup` script:
```
./RIOTDIR/dist/tools/tapsetup/tapsetup
```

2. Assign a site-global prefix to the `tabbr0` interface (the name could be
   different on OSX etc):
```
sudo ip a a fec0:affe::1/64 dev tapbr0
```

3. Assign a site-global address with the same prefix to the RIOT `native`
   instance:
```
ifconfig 5 add fec0:affe::99
```


## Usage
This example maps all available MQTT-SN functions to shell commands. Simply type
`help` to see the available commands. The most important steps are explained
below:

- To connect to a broker, use the `con` command:
```
con fec0:affe::1 1885
```

- To subscribe to a topic, run `sub` with the topic name as parameter, e.g.
```
sub hello/world
```

- For publishing, use the `pub` command:
```
pub hello/world "One more beer, please."
```

That's it, happy publishing!
