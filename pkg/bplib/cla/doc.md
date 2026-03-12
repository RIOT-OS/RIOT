@defgroup pkg_bplib_cla CLA Implementations
@ingroup  pkg_bplib
@brief    Default CLA implementations for bplib.

## Usage

To actually send data over the bundle protocol, an underlying "convergence layer"
and it's adapter has to be used, the CLA.

A CLA can be anything as long as it can send and receive data. There are CLAs
for direct sending over Ethernet but also CLAs over SMTP.

The following CLAs are implemented:

### UDPCL

@see pkg_bplib_cla_udp

@see https://datatracker.ietf.org/doc/draft-ietf-dtn-udpcl/03/

The (legacy) UDPCL sends bundles directly as UDP payload.


### BLE L2CAP CoC CLA

@see pkg_bplib_cla_ble

This CLA sends bundles directly as payload over an L2CAP connection.

In this basic implementation the roles of client / server (peripheral / central) are fixed,
the client connects to a statically known MAC address. Once the connection is established though
the server can also send data to the client. Note that the server does not confirm the MAC address
of the client as of now, it will accept any incoming connection.

This CLA is meant for testing and as a base point for a "good" CLA implementation, which also
allows discovery of devices and their reachable EIDs and so on.