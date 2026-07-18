---
title: Message APIs
description: Create and serialize CoAP messages.
---

`unicoap_message_t` is the central container type for CoAP messages. To see how to access CoAP
options, refer to the message and server pages in this folder.

## Example

```c
unicoap_message_t message;

const char payload[] = "Hello, World!";
unicoap_request_init_string(&message, UNICOAP_METHOD_POST, payload, &options);
```

You can access the CoAP code through different views, including `message.method`,
`message.status`, or `message.signal`.
