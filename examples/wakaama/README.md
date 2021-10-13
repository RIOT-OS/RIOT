# Wakaama LwM2M example client

This application starts a
[LwM2M](https://wiki.openmobilealliance.org/display/TOOL/What+is+LwM2M) client
on the node with instances of the following objects:
- [Security object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Security-v1_0.xml)
- [Server object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Server-v1_0.xml)
- [Device object](http://www.openmobilealliance.org/tech/profiles/LWM2M_Device-v1_0_3.xml)

The application is based on the Eclipse Wakaama
[example client](https://github.com/eclipse/wakaama/tree/master/examples/client)
.

## Usage

### Setting up a LwM2M Test Server
To test the client a LwM2M server where to register is needed.
[Eclipse Leshan](https://github.com/eclipse/leshan) demo is a good option for
running one locally.

To run the demo server:
```shell
wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar

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

#### Bootstrap server
LwM2M provides a bootstrapping mechanism to provide the clients with information
to register to one or more servers. To test this mechanism both the previous server and a bootstrap server should be running. Eclipse Leshan also provides a bootstrap server demo.

By default the bootstrap server option is disabled, it can be enabled by defining
`CONFIG_LWM2M_BOOTSTRAP` as 1 (see the Makefile in this application).

To run the bootstrap server, make sure that the ports it uses are different
from the ones of previous server (default are 5683 for CoAP, 5684 for CoAPs,
and 8080 for the webserver), and that it corresponds to the one set in
`lwm2m.h` as `CONFIG_LWM2M_BSSERVER_PORT`:
```shell
# download demo
wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-bsserver-demo.jar

# set CoAP, CoAPs and webserver ports for bootstrap server
BS_COAPPORT=5685
BS_COAPSPORT=5686
BS_WEBPORT=8888

# run the server
java -jar ./leshan-bsserver-demo.jar --coapport ${BS_COAPPORT} \
            --coapsport ${BS_COAPSPORT} --webport ${BS_WEBPORT}
```

To set up the configuration of the node and the server:
1. Click the `Add new client bootstrap configuration` button.
2. Fill in the name of the device, it **should** match the one set in
   `lwm2m.h` as `CONFIG_LWM2M_DEVICE_NAME`.
3. Using the `LWM2M Server` tab enter the address where the LwM2M server is
   listening. For now only `No security` mode can be used.

### Running the client
The address set as `CONFIG_LWM2M_SERVER_URI` (in `lwm2m.h` or via `menuconfig`)
should be reachable from the node, e.g. either running on native with a tap
interface or as a mote connected to a
[border router](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router).

Also, if a bootstrap server is being used the macro `CONFIG_LWM2M_BOOTSTRAP` should be
defined as 1.

The server URI for the example is being defined using the variable `SERVER_URI`
in the Makefile, and can be changed when compiling.

#### Configure, compile and run

The Wakaama package can be configured via Kconfig. Its options are placed
under `Packages > Configure Wakaama LwM2M`. To access the configuration
interface you can run:
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
