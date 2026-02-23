@defgroup net_unicoap_resources_xfa Static Resource Definitions
@ingroup  net_unicoap_server
@brief    Define CoAP resources across files at compile time
@{

Module. Specify `USEMODULE += unicoap_resources_xfa` in your application's Makefile.

Use @ref UNICOAP_RESOURCE to define resources across files as follows. The name you supply for
the resource must be unique among resource definitions but has otherwise no meaning. It is
needed purely for technical reasons involving the implementation of cross file arrays.
The resource macro must be followed by a constant @ref unicoap_resource_t initializer.
In the example below, we define
a resource reachable under /hello-world using the designated initializer.

```c
UNICOAP_RESOURCE(hello_world_resource) {
  .path = UNICOAP_PATH("hello-world"),
  .handler = my_hello_world_handler,
  .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET)
};
```

@}
