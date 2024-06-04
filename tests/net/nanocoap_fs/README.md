# nanoCoAP remote fs example

This allows to mount a remote fs that was exported via e.g. `nanocoap_fileserver`
or `aiocoap-fileserver`.

The test provides a `mount` command to mount a remote fs at a local mount point:

    mount coap://[fe80::607f:b1ff:fef7:689c]/vfs /coap

This will mount the `vfs/` resource on the remote server as a local `/coap/`
directory. This can then be interacted with normal `vfs` commands such as `ls`.
