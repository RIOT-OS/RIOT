# nanocoap_fileserver test

Unit tests for the nanocoap fileserver (`nanocoap_fileserver_handler()`).
Requests are built in memory and passed directly to the handler, so no
network is needed. The test runs on `native`, as it needs a writable file system.
