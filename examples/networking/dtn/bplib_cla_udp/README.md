# DTN example using bplib

[bplib](https://github.com/nasa/bplib) is a DTN implementation by NASA.

[What is DTN?](https://en.wikipedia.org/wiki/Delay-tolerant_networking) Delay- or
Disruption Tolerant Networking

- A network architecture that handles high communication delays and connection disruptions.
- Origination from space communication contexts for unreliable and/or very delayed connections.
- The protocol is called the *Bundle Protocol*, the version of bplib implements BPv7.

### How it this done?

The messages of the protocol (*bundles*) are sent over some underlying protocol,
called a *Convergence Layer* (CL). The object that translates between the underlying
protocol and the bundle processor is called a *Convergence Layer Adapter* (CLA).

CLAs exist for different protocols, this example shows the use of UDP as the CL
protocol, bundles are sent directly as UDP payloads.

The special thing is that DTN does not require an end-to-end connection between
two nodes and instead relies on store-and-forward. Each node should thus have
a persistent storage to store bundles which cannot currently be forwarded, because
there is no next hop.

### Usage of this example

This example was tested on the `native` target with gnrc UDP. It might also be usable
with 802.15.4 or nimble_netif to carry UDP, to test the communication on real hardware.
For direct communication over BLE, the [BLE example](../bplib_cla_ble/README.md) exists.

To configure this example, the Makefile processes the `REMOTE`, `REMOTE_EID_NODE`
and `LOCAL_EID_NODE` flags.

- `REMOTE` sets the underlying IPv6 remote address to send the bundles on
- `REMOTE_EID_NODE` sets the node number of the destination EID
- `LOCAL_EID_NODE` sets the node number of the current device

The service number of the EID is statically chosen in the example, as is the UDP
port (which by IANA considerations should use 4556 for BP/UDP anyways).

## Examples
### Loopback

To test simple loopback, nothing needs to be changed, the flags will default
to the loopback. Simply compile with `make all term`. Make sure a tap device is
available, since it tries to bind to one anyways.

When using the `bp send 0 "[DATA]"` shell command, the data you entered goes though bplib's
internal delivery, since the current node is the destination. The data is never
passed through gnrc in this case.

The 0 here represents the channel to send data on, in this case we only use one
channel, and indexing begins at 0.

### 2 devices (A <-> B)

To test with two devices create two tap devices as described in the gnrc examples.
Start both instances initially with `make all term PORT=tap0` and
`make all term PORT=tap1` and run the `ifconfig` shell command to find the IPv6
address of both virtual tap devices. Now that you know these, compile again to set
the remote addresses like so:
```shell
# Device A, has local address fe80::4832:95ff:feb7:5161
make all term PORT=tap0 REMOTE=fe80::d8af:c5ff:febc:a409 LOCAL_EID_NODE=100 REMOTE_EID_NODE=200
# Device B, has local address fe80::d8af:c5ff:febc:a409
make all term PORT=tap1 REMOTE=fe80::4832:95ff:feb7:5161 LOCAL_EID_NODE=200 REMOTE_EID_NODE=100
```

When running `bp send 0 "[DATA]"` now, the bundle should be delivered to the other instance.
Now, data goes through bplib, through gnrc, which sends the bundles as UDP payload.
The other device receives this and passes it to bplib again, which can deliver it,
since the channel is active

#### Disruption simulation

Since DTN is made for disruptions and thus employs a storage, this can be tested
as well in this example. On one (or both) of the nodes, run `bp contact 0 0` to
set the contact 0 into the stopped state. In the normal startup in this example
it will get put into the started state. Now bplib must assume that the contact
cannot currently serve its destinations.

Bundles that are sent *now* with `bp send 0 "[DATA]"` will not be received by the
other device. Instead, they are placed into the *storage*, which on native is located
in the `native` subfolder of this example.

When this disruption is over, the contact can be re-enabled with `bp contact 0 1`.
After some timeout, the bundles will be pulled from storage and are received at
device B.

How such a disruption can be detected reliably is unfortunately not trivial. DTN
originates from space communication, where contacts are schedulable from the static
and predictable orbits, but with IoT devices this is generally not the case.

### 3 devices (A.1 <-> B <-> A.2)

Using three devices would require larger changes to this example, however three BP
nodes (on two devices) can be simulated by reconfiguration.
Now device A wants to send a message to a different destination, which is neither
A nor B currently:

```bash
# Device A. Note the remote node id changed
make all term PORT=tap0 REMOTE=fe80::d8af:c5ff:febc:a409 LOCAL_EID_NODE=100 REMOTE_EID_NODE=300
# Device B
make all term PORT=tap1 REMOTE=fe80::4832:95ff:feb7:5161 LOCAL_EID_NODE=200 REMOTE_EID_NODE=100
```

When sending a message from A now, nothing is received at B, because B's local node
is not the specified destination. The message *does* get transmitted multiple times
between the nodes though, because both nodes are configured to send all bundles
with node ids 1 to 10000 to the other device.

This can be seen in Wireshark. The back and forth forwarding of the bundle here
only stops, because the Hop Limit block is added and the bundle is deleted after
the specified number of hops (10) is reached.

To now simulate the third node, stop the contact on device B with `bp contact 0 0`
and send a new bundle from A.
Now in Wireshark, only one bundle is sent and it is stored in the storage of B (`native200/`).

Now change device A such that it looks like it is a third device with the local node
number of the destination that A had previously:
```bash
# Device A. Note the remote node id changed again
make all term PORT=tap0 REMOTE=fe80::d8af:c5ff:febc:a409 LOCAL_EID_NODE=300 REMOTE_EID_NODE=100
```

When the contact is now enabled again on device B (`bp contact 0 1`), node A, which
now is a different node in the bundle context, receives the bundle after it is pulled
from the storage of B.

This demonstrated the core idea of DTN, store-and-forward transfer without direct
end-to-end paths. To forward between nodes without reconfiguration:
- Another IP device has to exist (here another tap device)
- The middle device has to have two contacts (`BPLIB_MAX_NUM_CONTACTS` >= 2)
- Two UDP CLAs have to be initialized to the respective next hops.
