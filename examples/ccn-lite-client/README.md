# examples/ccn-lite-client
This application is a showcase of the CCN capabilities in RIOT.
The demo consists of two parts, a client to fetch data, the ccn network stack and two ways of publishing content.
This part of the demo demonstrates the client functions, which comprise the network stack and a subscriber and publisher of content.

The demo starts the CCN-lite network stack and opens a shell to control the following entities.
The command `ccn X` starts the CCN stack and sets the content store size to X.

The demo has two ways of publishing content:

1. by filling the cache directly:  To pre-provision the cache with X dummy chunks use `populate`.

2. by starting a userland server application: An independent application `appserver` is also provided. This application starts a thread to answer to interests.
When the server is started, it will register a prefix to the stack. The server will reply to interests for content with the name `/riot/appserver/test`.

Subscriber: To fetch a file from the CCN network, execute the command `interest`. This starts a user land application, which creates an interest message and routes this message hop by hop towards the publisher of the content using the CCN stack.
