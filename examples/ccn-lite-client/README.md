This application is a showcase of the Content Centric Networking (CCN) capabilities in RIOT.

The example consists in two parts: clients and relays.

This file documents the client functionalities.

A client opens a shell to enable the following:

* **start the CCN stack**: The command `ccn X` starts the CCN stack and sets the content store size to X chunks. Chunk size is by default set to 802.15.4 packet payload (set it differently if needed). Note that X must be bigger or equal to 15 if you plan to publish content using Way 1 described below.

* **publishing content locally**: Two alternative ways are defined to do that.

 * *Way 1*: pre-provision the local CCN stack cache with X dummy chunks, with the command `populate`. Chunk size is by default set to 802.15.4 packet payload (set it differently if needed). The default name of the content is `/riot/text`.
 * *Way 2*: start a application in a different thread with the command `appserver`. This application will register a prefix `/riot/appserver/test` in the CCN network stack Forwarding Information Base (FIB). Note that from the view of the CCN network stack, there is no difference between local faces connecting to local appplications and network faces connecting to real network interfaces. When the application recieves an interest it replies with chunck `/riot/appserver/test/0`. Note that actual interest parsing is not implemented in the application.

* **subscribing to receive content**: the command `interest` fetches a remote file through the CCN network stack. This starts an application executed by the shell thread, which creates an interest message and routes this message hop by hop towards the publisher of the content (or a cache containing a copy) using the CCN stack. By default the content being fetched is `/riot/text`. One can also specify a name of content to fetch with the command `interest valid-name`. By valid, it is meant that the name should have structure like `/a/b/c` and the application will then request chunks with syntax `/a/b/c/0` etc.
