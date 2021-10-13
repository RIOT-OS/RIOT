## About

This is an additional gcoap example, but with enabled DTLS. It only provides a
custom configured makefile, while the code is a symlink to the original gcoap
example. Therefore, the infos and usage notes of the other README also applies to this
example.

Please note, that with DTLS the default port is 5684 and not 5683, thus CoAP requests
must be sent to this port.

Since DTLS has higher memory and and ROM requirements, more boards are blacklisted
for this example compared to the non-DTLS gcoap example.
