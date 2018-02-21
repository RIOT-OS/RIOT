Eclipse Wakaama Example Client
==============================

This application is a port of the Eclipse Wakaama [example client
code](https://github.com/eclipse/wakaama/tree/master/examples/client)
to RIOT/GNRC.

Usage
=====

Edit the defines in `lwm2mconfig.h` according to your setup (see the
comments for what they do).

By setting `LWM2M_SERVER_PSK_ID` and `LWM2M_SERVER_PSK_KEY` to NULL in
`lwm2mconfig.h`, CoAP will be used for communication instead of CoAPS.

Build, flash and start the application:
```
export BOARD=your_board
make all flash term
```

(Or Skip `make flash` if you're building for `BOARD=native`.)

The `term` make target starts a terminal emulator for your board. It
connects to a default port so you can interact with the shell, usually
that is `/dev/ttyUSB0`. If your port is named differently, the
`PORT=/dev/yourport` (not to be confused with the UDP port) variable can
be used to override this.


Available LwM2M Objects
=======================

At this time, only the mandatory objects for a client are present. The
*server*, *security* and *access control* objects are taken as is from
the example client in the Wakaama source tree. The *device* object was
re-implemented for RIOT (see `object_device.c`).

Adding more objects should be as easy as implmenting a "contructor"
and then incrementing the `OBJ_COUNT` in `lwm2mclient.c` and extending
`run_server()` to call the constructor and adding the instance(s) to
`objArray`.


LwM2M Test Server
=================

[Eclipse Leshan](https://github.com/eclipse/leshan#test-leshan-demos-locally)
is good for local testing. Just run it on the link local address of
the bridge interface created by `dist/tools/tapsetup/tapsetup` and add
the credentials set in `lwm2mconfig.h` via the web interface. Since the
example bootstrap server doesn't seem to support setting DTLS credentials
for the server itself via the web interface, you'll have to create a
new client config, edit `data/bootstrap.json` and set `securityMode`,
`publicKeyOrId`, `secretKey` and maybe `serverPublicKey` and restart
the server.

The LwM2M demo server supports setting DTLS credentials on the *Security*
tab.

Example (Linux):
```
wget https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar

ADDR=$(ip address show dev tapbr0 | grep inet6 |\
   head -1 | awk '{print $2}' | cut -d'/' -f1)

java -jar ./leshan-server-demo.jar \
   --coapshost ${ADDR} \
   --coaphost ${ADDR}
```

Example bootstrap.json
----------------------

An example `bootstrap.json` file for the Leshan bootstrap server can be
found in data `data/`.

`testRIOTDevice.servers` contains a list of LwM2M
server objects that should be installed on the client
*testRIOTDevice*. `testRIOTDevice.servers.*.shortId` must match a
corresponding `testRIOTDevice.security.*.shortId`.

`testRIOTDevice.security` contains a list of LwM2M
security objects. `testRIOTDevice.security.0` in this
example specifies the bootstrap server itself and its
DTLS PSK. `testRIOTDevice.security.0.publicKeyOrId` and
`testRIOTDevice.security.0.secretKey` in this case are equivalent to
`fooTestRIOT` and `39fe6611deb7713c6069`.

`testRIOTDevice.security.1` matches the server to install on the
client. As mentioned, it's `shortId` must match the corresponding entry
in `testRIOTDevice.servers`. For simplicity's sake its PSK is identical
to the bootstrap server.


Example output
==============

For this example, set:
* `ENABLE_DEBUG=(1)` in `lwm2mconfig.h`
* `CFLAGS += -DLWM2M_WITH_LOGS` in `Makefile`
* `CFLAGS += -DDTLS_LOG_LEVEL=DTLS_LOG_CRIT` in `Makefile`

When registering directly to an LwM2M server, with DTLS, without
boostrapping, this should output something similar to this:
```
main(): This is RIOT! (Version: 2018.01-devel-943-g987f6-calamity-track1/wakaama-integration)
[lwm2m_init:64] Entering
[lwm2m_configure:264] endpointName: "testRIOTDevice", msisdn: "(null)", altPath: "(null)", numObject: 4
 -> State: STATE_INITIAL
[lwm2m_step:372] timeoutP: -616140018120916932
[lwm2m_step:377] State: STATE_INITIAL
[object_getServers:741] Entering
[lwm2m_data_new:143] size: 3
[lwm2m_data_encode_bool:406] value: false
[lwm2m_data_encode_int:270] value: 10
[lwm2m_data_encode_int:270] value: 10
[lwm2m_data_decode_bool:416] Entering
[lwm2m_data_decode_bool:467] result: 1, value: false
[lwm2m_data_decode_int:280] Entering
[lwm2m_data_decode_int:335] result: 1, value: 10
[lwm2m_data_new:143] size: 1
[lwm2m_data_encode_int:270] value: 10
[lwm2m_data_decode_int:280] Entering
[lwm2m_data_decode_int:335] result: 1, value: 10
[lwm2m_data_free:161] size: 1
[lwm2m_data_new:143] size: 2
[lwm2m_data_encode_int:270] value: 300
[lwm2m_data_encode_string:195] "U"
[lwm2m_data_decode_int:280] Entering
[lwm2m_data_decode_int:335] result: 1, value: 300
[lwm2m_data_free:161] size: 2
[lwm2m_data_free:161] size: 3
[registration_start:477] State: STATE_REGISTER_REQUIRED
[object_getRegisterPayloadBufferLength:508] Entering
[object_getRegisterPayload:579] Entering
[lwm2m_data_new:143] size: 1
[lwm2m_data_encode_string:195] "coaps://[fe80::585d:b5ff:fe1e:75]"
[lwm2m_data_free:161] size: 1
[lwm2m_data_new:143] size: 1
[lwm2m_data_encode_int:270] value: 0
[lwm2m_data_decode_int:280] Entering
[lwm2m_data_decode_int:335] result: 1, value: 0
[lwm2m_data_free:161] size: 1
[transaction_new:156] method: 2, altPath: "(null)", mID: 30050, token_len: 4
[transaction_new:157] NULL
[transaction_new:235] Exiting on success
[transaction_send:351] Entering
Success: started DTLS server on port 61618
> [observe_step:482] Entering
[registration_step:1303] State: STATE_REGISTERING
[transaction_step:433] Entering
[lwm2m_step:480] Final timeoutP: -616140018120916990
[lwm2m_step:482] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
[lwm2m_data_new:143] size: 1
[lwm2m_data_encode_opaque:232] length: 11
[lwm2m_data_free:161] size: 1
[lwm2m_data_new:143] size: 1
[lwm2m_data_encode_opaque:232] length: 10
[lwm2m_data_free:161] size: 1
decrypt_verify(): found 24 bytes cleartext
 -> State: STATE_REGISTERING
[lwm2m_step:372] timeoutP: 679896346613776444
[lwm2m_step:377] State: STATE_REGISTERING
[registration_getStatus:506] State: STATE_REGISTERING
[registration_getStatus:513] targetP->status: STATE_REG_PENDING
[registration_getStatus:536] reg_status: STATE_REG_PENDING
[observe_step:482] Entering
[registration_step:1303] State: STATE_REGISTERING
[transaction_step:433] Entering
[transaction_send:351] Entering
[lwm2m_step:480] Final timeoutP: 679896346613776387
[lwm2m_step:482] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
decrypt_verify(): found 22 bytes cleartext
[lwm2m_handle_packet:214] Entering
[lwm2m_handle_packet:219] Parsed: ver 1, type 2, tkl 4, code 2.01, mid 30050, Content type: 0
[lwm2m_handle_packet:220] Payload:
[transaction_handleResponse:274] Entering
[prv_handleRegistrationReply:212] Registration successful
[transaction_remove:260] Entering
[transaction_free:246] Entering
 -> State: STATE_REGISTERING
[lwm2m_step:372] timeoutP: 679896346613776444
[lwm2m_step:377] State: STATE_REGISTERING
[registration_getStatus:506] State: STATE_REGISTERING
[registration_getStatus:513] targetP->status: STATE_REGISTERED
[registration_getStatus:536] reg_status: STATE_REGISTERED
[observe_step:482] Entering
[registration_step:1303] State: STATE_READY
[transaction_step:433] Entering
[lwm2m_step:480] Final timeoutP: 679896346613776444
[lwm2m_step:482] Final state: STATE_READY
```


Limitations
===========

* The host part of any URI **MUST** be a valid IPv6 address, as the
  client can't resolve host names at this time.
