@defgroup net_unicoap_message Message APIs
@ingroup  net_unicoap
@brief    Create and serialize CoAP messages
@{

@ref unicoap_message_t is the central container type for CoAP messages. To see how to access
CoAP options, see @ref net_unicoap_options. You may also look at the [guide to using CoAP
messages](https://guides.riot-os.org/FIXME)

## Example

```c
unicoap_message_t message;

const char payload[] = "Hello, World!";
unicoap_request_init_string(&message, UNICOAP_METHOD_POST, payload, &options);
```

You can access the CoAP code through different views, including as ast
@ref unicoap_message_t.method,
@ref unicoap_message_t.status, or
@ref unicoap_message_t.signal number.

@}
