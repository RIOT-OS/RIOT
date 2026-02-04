# Wakaama LwM2M example client

This application starts a
[LwM2M](https://wiki.openmobilealliance.org/display/TOOL/What+is+LwM2M) client
on the node with instances of the following objects:
- [Security object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Security-v1_0.xml)
- [Server object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Server-v1_0.xml)
- [Device object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Device-v1_0_3.xml)
- [Light control object](https://raw.githubusercontent.com/OpenMobileAlliance/lwm2m-registry/prod/3311.xml)

The application is based on the Eclipse Wakaama
[example client](https://github.com/eclipse/wakaama/tree/master/examples/client).

## Usage

### Setting up a LwM2M Test Server
To test the client a LwM2M server where to register is needed.
[Eclipse Leshan](https://github.com/eclipse/leshan) demo is a good option for
running one locally.

To run the demo server, download the jar file:
```shell
wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar
```

And run it:
```shell
java -jar ./leshan-server-demo.jar
```
It will output the addresses where it is listening:
```
INFO LeshanServer - LWM2M server started at coap://0.0.0.0/0.0.0.0:5683 coaps://0.0.0.0/0.0.0.0:5684
INFO LeshanServerDemo - Web server started at http://0.0.0.0:8080/.
```

Additionally, you can specify a binding address for the Leshan server, using
the `-lh` option. For instance, if you are connecting from a node via a Border
Router, you might want to specify:

```shell
java -jar ./leshan-server-demo.jar -lh fd00:dead:beef::1
```

In the security section click 'Add Security Information', select the security mode
'Pre-Shared Key', and enter the Client endpoint name and the security information
(Identity and Key).

#### Bootstrap server
LwM2M provides a bootstrapping mechanism to provide the clients with information
to register to one or more servers. To test this mechanism both the previous server and a bootstrap server should be running. Eclipse Leshan also provides a bootstrap server demo.

By default the security instance created in the application assumes that a standard LwM2M Server is
used. To indicate that the configuration corresponds to a LwM2M Bootstrap Server, set the
corresponding argument (`is_bootstrap`) to true. Also, bootstrap support needs to be enabled in the
wakaama package configurations. This can be done via `make menuconfig` or by setting the CFLAG
`CONFIG_LWM2M_BOOTSTRAP`.

To run the bootstrap server, make sure that the ports it uses are different
from the ones of previous server (default are 5683 for CoAP, 5684 for CoAPs,
and 8080 for the webserver), and that it corresponds to the one set in
`lwm2m.h` as `CONFIG_LWM2M_BSSERVER_PORT`.

Download the jar file:
```shell
wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-bsserver-demo.jar
```

And run it:
```shell# download demo
# set CoAP, CoAPs and webserver ports for bootstrap server
BS_COAPPORT=5685
BS_COAPSPORT=5686
BS_WEBPORT=8888

# run the server
java -jar ./leshan-bsserver-demo.jar --coap-port ${BS_COAPPORT} \
            --coaps-port ${BS_COAPSPORT} --web-port ${BS_WEBPORT}
```

To set up the configuration of the node and the server:
1. Click the `Add new client bootstrap configuration` button.
2. Fill in the name of the device, it **should** match the one set as `CONFIG_LWM2M_DEVICE_NAME`,
   in `objects/device.h`.
3. Using the `LWM2M Server` tab enter the address where the LwM2M server is
   listening. Here you can select `No security` or `Pre-Shared Key` modes.

### Running the client
The server address is set by the application, during the instantiation of the Security object.
It can be set via `menuconfig` or the environmental variable `LWM2M_SERVER_URI`. It should be
reachable from the node, e.g. either running on native with a tap interface or as a mote connected
to a
[border router](https://github.com/RIOT-OS/RIOT/tree/master/examples/networking/gnrc/border_router).

Also, if a bootstrap server is being used, it should be configured in the application via
`menuconfig` or setting the environmental variable `LWM2M_SERVER_BOOTSTRAP` to 1. This information
is used in the Security object instance.

#### Configure, compile and run

The Wakaama package can be configured via Kconfig. Its options are placed
under `Packages > Configure Wakaama LwM2M`. There is also an application-specific configuration
menu. There the Server URI and credentials can be set. To access the configuration interface you
can run:
```
make menuconfig
```

For debugging purposes there are two types of messages that can be enabled:
- The lwm2m client adaptation debug can be enabled by setting `ENABLE_DEBUG` in
  `lwm2m_client.c` and `lwm2m_client_connection.c` to 1
- The wakaama internal logging can be enabled by setting
  `CONFIG_LWM2M_WITH_LOGS`, either on `menuconfig` or by adding it to CFLAGS
  (`CFLAGS += -DCONFIG_LWM2M_WITH_LOGS`).

For memory allocation the TLSF package is being used, with a private heap. If
memory usage has to be tweaked the heap size can be modified via the option
`CONFIG_LWM2M_TLSF_BUFFER`.

To compile run:

```shell
BOARD=<board> make clean all flash term
```

#### Shell commands
- `lwm2m start`: Starts the LwM2M by configuring the module and registering to
  the server.
- `lwm2m light <on|off> <dimmer> [color]`: Sets the light state to on or off,
  with a given dimmer value and optional color.
- `lwm2m mem`: (Only available if `DEVELHELP` is enabled in your Makefile) Prints the memory
  usage of the LwM2M module.
