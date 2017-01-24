IoTivity Examples
===============
These examples implement some simple clients and a server to test the IoTivity package for RIOT-OS. The pkg is based on the [IoTivity-Constrained][1] library.
All examples use the realm-local multicast address **ff03:158** instead of the link-local multicast address **ff02::158**. Every payload is [CBOR][2] encoded.
All examples have been tested on Native and SAMR21-XPRO.
##Index
 - [Iotivity Examples](#examples)
   - [Server Example](#server_ex)
   - [Client Example](#client_ex)
   - [Client_Switch Example](#client_sw_ex)
   - [BR_FW Example](#br_fw_ex)
 - [Scenarios](#scenarios)
   - [Node-to-Node Communications](#n2n_comm)
     -  [Server and Client (Periodic PUT) - native target](#sc_pput_native)
     -  [Server and Client (Periodic PUT) - SAMR21-XPRO target](#sc_pput_samr21)
     -  [Server and Client_Switch  - SAMR21-XPRO target](#sc_sw_samr21)
   - [Linux-to-Nodes Communications](#l2n_comm)
     - [Preliminary Step](#l2n_pre)
     - [Start the Server](#l2n_srv)
     - [Start the Border Router](#l2n_br)
     - [Server output](#l2n_out)
     - [Testing](#l2n_tst)
 - [Good Practice](#good)

#<a name="examples"></a>Examples

##<a name="server_ex"></a>Server Example
This example implements an IoTivity Server that contains 4 resources.
 1. */oic/res*: returns the representation of resources available on the server.  Defined [here][3].
 2. */oic/p*: returns information about the platform on which the server runs. Defined [here][3].
 3. */oic/d*: returns information about the device on which the server runs. Defined [here][3].
 4. */light/1* : it is a light (LED) management resource (implemented in resources/res-light.c). Resource Type: *oic.r.light*.
	- GET: returns the status of the LED. The payload is: `{state: true/false}`
	- PUT: changes the status of the LED. The payload is: `{state: true/false}`
	- OBSERVE: the registered client is notified when the status changes. The payload is the same of the GET response.

## <a name="client_ex"></a>Client Example
This example implements a simple client. It is able to discover resources with ResourceType *oic.r.light*. Once it finds a resource, it registers for OBSERVE notifications. It changes the status every second by sending a periodic PUT request.

##<a name="client_sw_ex"></a>Client_Switch Example
This example implements a simple client. It is able to discover resources with ResourceType *oic.r.light*. Once it finds a resource, it registers for OBSERVE notifications. It changes the status when the User Button is pressed. If the button  is not present it is just an observer.

##<a name="br_fw_ex"></a>BR_FW Example
It is an "enhanced" version of the GNRC Border Router. It implements a simple forwarder (UDP server/client) for multicast requests with destination ff03::158 port 5683.

#<a name="scenarios"></a>Scenarios
It is possible to deploy 2 different scenarios with these examples.

##<a name="n2n_comm"></a>Node-to-Node Communications
In this scenario, we will deploy an IoTivity Client and IoTivity Server on different nodes. We can choose two different clients for this scenario: client (periodic PUT) or client_switch (PUT sent on User Button pressed). The first one runs well both on native either on SAMR21-XPRO boards, the second one runs just on SAMR21-XPRO boards. Native target hasn't the button.

### <a name="sc_pput_native"></a>Server and Client (Periodic PUT) - native target
Create taps interfaces (to which RIOT will connect). Go to `/dist/tools/tapsetup` and type
```
$ sudo ./tapsetup -c
```
After this step we have created three tap interfaces: tap0, tap1 and tapbr0.
Now, we compile the server. Go to `/examples/iotivity-examples/server` and type
```
$ make all BOARD=native
```
Run the server by invoking
```
$ sudo bin/native/ocf_server.elf tap0
```
The server output will be similar to this
```
RIOT native interrupts/signals initialized.
LED_RED_OFF
LED_GREEN_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2017.01-devel-13-g2b77e-mattia-Latitude-E6410-pkg/iotivity)
server_oic: Waiting for address autoconfiguration...ipadapter: waiting for server requests...
ipadapter: waiting for multicast requests...
server_oic: LED0 is OFF
LED_RED_OFF
oc_main: Stack successfully initialized
server_oic: Configured network interfaces:
Iface  5   HWaddr: e6:e8:ff:6b:0c:f2

           MTU:1500  HL:64  RTR  RTR_ADV
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::e4e8:ffff:fe6b:cf2/64  scope: local
           inet6 addr: ff02::1:ff6b:cf2/128  scope: local [multicast]
           inet6 addr: ff02::2/128  scope: local [multicast]
           inet6 addr: ff02::1a/128  scope: local [multicast]
           inet6 addr: ff03::158/128  scope: global [multicast]
```
It is waiting for requests.
Open a new terminal window, go to `/examples/iotivity-examples/client` and type
```
$ make all BOARD=native
```
Run the client by invoking
```
$ sudo bin/native/ocf_client.elf tap1
```
The client runs and it starts with the discovery phase
```
RIOT native interrupts/signals initialized.
LED_RED_OFF
LED_GREEN_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2017.01-devel-13-g2b77e-mattia-Latitude-E6410-pkg/iotivity)
LED_RED_OFF
client_oic: Waiting for address autoconfiguration...ipadapter: waiting for server requests...
ipadapter: waiting for multicast requests...
oc_main: Stack successfully initialized
client_oic: Configured network interfaces:Iface  5   HWaddr: a6:ab:89:bd:1f:80

           MTU:1500  HL:64  RTR  RTR_ADV
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::a4ab:89ff:febd:1f80/64  scope: local
           inet6 addr: ff02::1:ffbd:1f80/128  scope: local [multicast]
           inet6 addr: ff02::2/128  scope: local [multicast]
           inet6 addr: ff02::1a/128  scope: local [multicast]
           inet6 addr: ff03::158/128  scope: global [multicast]

client_oic: continue discovery
Outgoing message to [ff03:0000:0000:0000:0000:0000:0000:0158]:5683
client_oic: continue discovery
...
ipadapter: got server request
Incoming message from [fe80:0000:0000:0000:241c:c8ff:fe14:3d79]:56789
ipadapter: waiting for server requests...
client_oic: Discovery done
client_oic: Ready...
LED_RED_ON
```
Once the resource is discovered, the client registers as an Observer of the resource and it switches on its LED as notification of Discovery Completed.
From this point it will send a PUT request every second.
Client Output:
```
client_oic: Sent PUT request
Outgoing message to [fe80:0000:0000:0000:241c:c8ff:fe14:3d79]:56789
ipadapter: got server request
Incoming message from [fe80:0000:0000:0000:241c:c8ff:fe14:3d79]:56789
ipadapter: waiting for server requests...
client_oic: PUT_light:
client_oic: PUT response OK
ipadapter: got server request
Incoming message from [fe80:0000:0000:0000:241c:c8ff:fe14:3d79]:56789
ipadapter: waiting for server requests...
client_oic: OBSERVE_light: key state, value 0
```
Server output:
```
Incoming message from [fe80:0000:0000:0000:a4ab:89ff:febd:1f80]:56789
ipadapter: waiting for server requests...
server_oic: PUT request
server_oic: key: state value: 0
server_oic: LED0 is OFF
LED_RED_OFF
Outgoing message to [fe80:0000:0000:0000:a4ab:89ff:febd:1f80]:56789
server_oic: GET request
server_oic: Light state 0
Outgoing message to [fe80:0000:0000:0000:a4ab:89ff:febd:1f80]:56789
```
TAPs interfaces can be easily deleted. Go to `/dist/tools/tapsetup` and type
```
$ sudo ./tapsetup -d
```

###<a name="sc_pput_samr21"></a> Server and Client (Periodic PUT) - SAMR21-XPRO target
Now, we reproduce the previous scenario using two [SAMR21-XPRO][4] nodes.
Connect your nodes, go to `/examples/iotivity-examples/server` and check the list of USB-connected nodes by typing:
```
$ make list-ttys
```
The output will be similar to
```
/sys/bus/usb/devices/2-1.3: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800001234', tty(s): ttyACM0
/sys/bus/usb/devices/2-1.4: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800004321', tty(s): ttyACM1
```
We will use Serial Numbers in order to identify the designed node during the compilation phase.
Now, we compile the server
```
$ make flash BOARD=samr21-xpro SERIAL=server_node_serial
```
then we open the serial connection
```
$ make term BOARD=samr21-xpro SERIAL=server_node_serial
```
The server starts and it is waiting for incoming requests.
Now,  open a new terminal window, go to `/examples/iotivity-examples/client` and type
```
$ make flash BOARD=samr21-xpro SERIAL=client_node_serial
$ make term BOARD=samr21-xpro SERIAL=client_node_serial
```
Client starts the discovery phase. Once it finds a resource (with ResourceType **oic.r.light**), it registers as an observer on the resource, then it switches on its LED and it finally starts with periodic PUT requests. The server LED will blink periodically.
Client and Server terminal outputs are similar to the outputs in case of native target.

###<a name="sc_sw_samr21"></a> Server and Client_Switch - SAMR21-XPRO target
This deployment emulates a smart home scenario in which we have a SmartBulb (server) and a SmartSwitch (client_switch). It requires two SAMR21-XPRO nodes or similar.
Connect your nodes, go to `/examples/iotivity-examples/server` and check the list of USB-connected nodes by typing:
```
$ make list-ttys
```
The output will be similar to
```
/sys/bus/usb/devices/2-1.3: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800001234', tty(s): ttyACM0
/sys/bus/usb/devices/2-1.4: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800004321', tty(s): ttyACM1
```
We will use Serial Numbers in order to identify the designed node during the compilation phase.

Now, we compile the server
```
$ make flash BOARD=samr21-xpro SERIAL=server_node_serial
```
then we open the serial connection
```
$ make term BOARD=samr21-xpro SERIAL=server_node_serial
```
The server starts and it is waiting for incoming requests.

Now, we open a new terminal window, go to `/examples/iotivity-examples/client_switch` and type
```
$ make flash BOARD=samr21-xpro SERIAL=client_node_serial
$ make term BOARD=samr21-xpro SERIAL=client_node_serial
```
Client performs the discovery phase. Once it is completed, client registers as an Observer of the resource, then it switches on its LED.
Client is now ready to send a PUT request when the User Button is pressed. The server LED will change the status when the button is pressed. Terminal outputs are similar to outputs in previous examples.

##<a name="l2n_comm"></a>Linux-to-Nodes communications
In this scenario, we will deploy an IoTivity server on a RIOT node and the IoTivity client will run on a Linux machine. This architecture requires the "enhanced" version of the Border Router [BR_FW](br_fw). It requires two SAMR21-XPRO nodes or similar.

###<a name="l2n_pre"></a>Preliminary step
Connect your nodes, go to `/examples/iotivity-examples/server` and check the list of USB-connected nodes by typing:
```
$ make list-ttys
```
The output will be similar to
```
/sys/bus/usb/devices/2-1.3: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800001234', tty(s): ttyACM0
/sys/bus/usb/devices/2-1.4: Atmel Corp. EDBG CMSIS-DAP serial: 'ATML2127031800004321', tty(s): ttyACM1
```
We will use Serial Numbers in order to identify the designed node during the compilation phase.

###<a name="l2n_srv"></a>Start the Server
Open a terminal window, go to `/examples/iotivity-examples/server` and type
```
$ make flash BOARD=samr21-xpro SERIAL=server_node_serial
```
then we open the serial connection
```
$ make term BOARD=samr21-xpro SERIAL=server_node_serial
```
The server starts the initialization phase, then it is ready for incoming requests.

###<a name="l2n_br"></a>Start the Border Router
Step 1) Open a terminal window in `/example/iotivity-examples/br_fw/` and type
```
$ make flash BOARD=samr21-xpro SERIAL=br_node_serial
```
Step 2) Once the flashing is finished, we have to open a network interface. Type
```
$  PORT=/dev/ttyACM0 make term
```
Step 3) We have to complete the routing strategy on the BR, so in the RIOT shell type
```
> ifconfig 7 add 2001:db8::2
> fibroute add :: via 2001:db8::1 dev 7
```
Now the BR is ready. The network is configured as follow:
- Serial Port: **/dev/ttyACM0**
- Interface: **tap0**
- Address Prefix: **2001:db8::\64**
- Routing on tap0 of multicast packets with destination **ff03::158**

It is possible to configure the network with different parameters by invoking directly the initialization script instead of executing the Step 2:
```
$ make host-tools
$ sudo ./start_network_mcast.sh <serial-port> <tap-device> <IPv6-prefix> <IPv6-mcast>
```
then run the Step 3 with the proper changes.

We can check the reachability of the server by typing in another terminal window
```
$ ping6 <IPv6 server>
```
The IPv6 address of the server can be found by typing in the Border Router console:
```
> routers
```
The output will be similar to
```
if  Router                          state      type
---------------------------------------------------
 6  2001:db8::5859:1c2a:64c7:c48a   REACHABLE   REG
```

###<a name="l2n_out"></a>Server Output
Managing a GET request the output is like
```
2016-11-04 14:39:32,115 - INFO # ipadapter: got server request
2016-11-04 14:39:32,121 - INFO # Incoming message from [2001:0db8:0000:0000:0000:0000:0000:0001]:56214
2016-11-04 14:39:32,125 - INFO # ipadapter: waiting for server requests...
2016-11-04 14:39:32,128 - INFO # server_oic: GET request
2016-11-04 14:39:32,130 - INFO # server_oic: Light state 0
2016-11-04 14:39:32,145 - INFO # Outgoing message to [2001:0db8:0000:0000:0000:0000:0000:0001]:56214
```
Managing a PUT request the output is like
```
2016-11-04 14:39:35,119 - INFO # ipadapter: got server request
2016-11-04 14:39:35,125 - INFO # Incoming message from [2001:0db8:0000:0000:0000:0000:0000:0001]:56214
2016-11-04 14:39:35,128 - INFO # ipadapter: waiting for server requests...
2016-11-04 14:39:35,134 - INFO # server_oic: PUT request
2016-11-04 14:39:35,136 - INFO # server_oic: key: state value: 1
2016-11-04 14:39:35,138 - INFO # server_oic: LED0 is ON
2016-11-04 14:39:35,141 - INFO # Outgoing message to [2001:0db8:0000:0000:0000:0000:0000:0001]:56214
```

###<a name="l2n_tst"></a>Testing
There are many different ways to test this scenario.

 - Tools: you can use [coap-cbor-cli][5] to perform get request. Put -c as argument.
 - Iotivity Client: you can write an iotivity client that runs on Linux. [Here][6] and [Here][7] there are some sample clients that can be used to test this scenario.

#<a name="good"></a>Good Practice
Discoveries and Requests are usually fast, but a timeout delay of some seconds on responses can be useful. In these examples, delays are set to 1 second for discoveries and they can be increased. For requests coming from a linux-based client, timeout delays should be higher (e.g. 5-10 seconds for discoveries and 3 seconds for requests). They can be tuned depending on the application. The minimum value is around 1 second.

[1]: https://github.com/iotivity/iotivity-constrained/
[2]: http://cbor.io/
[3]: https://openconnectivity.org/resources/specifications
[4]: http://www.atmel.com/tools/ATSAMR21-XPRO.aspx
[5]: https://github.com/domabo/coap-cbor-cli
[6]: https://github.com/Agile-IoT/agile-iotivity/tree/master/IoTivity/Simple-Client-Linux
[7]: https://github.com/Agile-IoT/agile-iotivity/tree/master/IoTivity/Interactive-Client-Linux
