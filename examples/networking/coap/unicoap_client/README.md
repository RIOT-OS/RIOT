# Sample Client Application With `unicoap`

This is a sample application demonstrating how you send CoAP requests using `unicoap`,
the unified and modular CoAP suite in RIOT. Visit the
[`unicoap` client tutorial on doc.riot-os.org](http://doc.riot-os.org/group__net__unicoap__client__tutorial.html)
for a detailed code-level instructions.

## Running the Example on RIOT's Native Board

Create a tap interface (to which RIOT will connect):

```sh
sudo ip tuntap add tap0 mode tap user ${USER}
sudo ip link set tap0 up
```

To try this example on your host, run:
```sh
BOARD=native make flash term
```
This will compile and run the application.

In a second terminal session, you can run

```sh
python3 server.py"
```

Now, you can send CoAP requests from RIOT through the tap interface to the CoAP server. The Python
script implements the following resources:
- `/time`: Responds with current timestamp
- `/other/separate`: Sends response in separate message instead of piggybacking (if request has been
transmitted reliably over UDP or DTLS at all)
- `/whoami`: Responds with requester information
- `/other/block`: Responds with bigger message, supports block-wise transfer (not supported yet)
