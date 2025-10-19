# Sample Server Application With `unicoap`

This a sample application demonstrating how you can define CoAP resources using `unicoap`,
the unified and modular CoAP suite in RIOT. Visit the
[`unicoap` server tutorial on doc.riot-os.org](http://doc.riot-os.org/group__net__unicoap__server__tutorial.html)
for a detailed code-level instructions.

## Running the Example on RIOT's Native Board

Create a tap interface (to which RIOT will connect):

```
$ sudo ip tuntap add tap0 mode tap user ${USER}
$ sudo ip link set tap0 up
```

To try this example on your host, run:
```sh
BOARD=native make flash term
```
This will compile and run the application.
The application will print a network-layer address.

In a second terminal session, you can run

```sh
python3 client.py -m GET -u "coap://[fe80::c0:ff:ee%tap0]/greeting?name=RIOTer"
```

to send a CoAP request through the tap interface to the CoAP server where `fe80::c0:ff:ee`
is the link-layer address printed by the application.
