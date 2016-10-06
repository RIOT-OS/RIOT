# ndn-ping

This application demonstrates the usage of the package ndn-riot.
This example basically enables the user to setup a ndn data server, and a ndn client that can request the data.
Any board with a default netdev can be used to run this example.

# Setting up for native

Create `tap` and `tapbr` devices using RIOT's `tapsetup` script before stating the application:
```bash
./RIOTDIR/dist/tools/tapsetup/tapsetup
```

Then run the application on 2 different terminals :
```bash
# on the first terminal
make PORT=tap0 term
# on the second terminal
make PORT=tap1 term
```

# Usage

The user can run shell commands (type "help" to see the list).
Only one command is relative to ndn : `ndnping`.

## Start a server

```
ndnping server name_uri server_id
```

Replace `name_uri` by a ndn name (for example `/test`), and `server_id` by a number.
`server_id` will be appended to the name of the data sent by the server.
This can help when several servers are running using the same `name_uri`, but is not useful in our example.

A server will start and answer to any interest message matching the name.

## Start a client

```
ndnping client name_uri max_count
```

Replace `name_uri` by a ndn name, and `max_count` by a number.
`max_count` is the number of interest message that will be sent.

A client will start, send a first interest message and wait for a data message.
Once data is received or timeout is reached, the client can send the next interest message, or stop when the last interest have been sent.
