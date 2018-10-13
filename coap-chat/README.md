## About

This application provides a simple command line chat using (g)CoAP as transport.
It allows to send (short) messages to any (IPv6) address reachable by the node.
All messages are sent to and received for a CoAP resource with path `/chat`.

## Usage

To send messages use the `chat` shell command, it can be invoked as follows:

```
chat <destination> <nickname> <message>
```

- **destination:** a reachable IPv6 address, may even be multicast `ff02::1`
- **nickname:** your chat nickname, anything is allowed, but keep it short
- **message:** what ever text you want to share, be brief, size is limited

The message format is plain text and will be send as *nickname: message*, the
maximum message size is 63 chars - so keep *nickname*  and *message* short :)
Please be aware that all CoAP messages are sent as non-confirmable, hence there
is no retransmission in case of packet loss.

## Notes

The code base of this application aims for simplicity, thus it only provides
a minimalistic set of functions. An advanced CoAP application can be found in
the [RIOT examples](https://github.com/RIOT-OS/RIOT/tree/master/examples/gcoap).

The application also ships with a number of standard shell commands, such as
`ifconfig` to allow for network interface configuration. Just type `help` in
the shell to see a full list and description of all available commands.

By default this application is configured to run as an IPv6 node, i.e. it does
not include any routing or relaying functionality - such needs to be handled
by other nodes.
