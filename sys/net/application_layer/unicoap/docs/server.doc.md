@defgroup net_unicoap_server CoAP Server
@ingroup  net_unicoap
@brief    Register CoAP resources and handle requests asynchronously
@{

A CoAP resource behaves similar to an HTTP endpoint.

## Registering a Resource

You can either declare a resource using static cross-file arrays (1) or manually at runtime (2).

1. This technique requires the @ref net_unicoap_resources_xfa module (XFA = cross-file arrays).
   The identifier you specify with the @ref UNICOAP_RESOURCE macro just needs to be unique,
   but isn't used otherwise. You will need to specify a path and a set of allowed request methods
   using @ref UNICOAP_METHODS.

   ```c
   UNICOAP_RESOURCE(my_resource) {
     .path = "/led/state",
     .handler = my_request_handler,
     .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_PUT)
   }
   ```

2. Alternatively, you can create a new listener and manually add resources as follows.
   Listeners are groups of resources. Internally, all XFA resources form a built-in listener.

   ```c
   // First, declare your resources in an array
   static unicoap_resource_t my_resources[] = {
      {
        .path = "/led/state",
        .handler = my_request_handler,
        .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_PUT)
      }
   };

   // Then, create a listener encompassing your resources
   static unicoap_listener_t my_listener = {
     .resources = my_resources,
     .resource_count = ARRAY_SIZE(my_resources),
     .request_matcher = unicoap_resource_match_request_default,
   };

   // Finally, register your listener with unicoap
   unicoap_listener_register(&my_listener);

   // Optionally, deregister your listener later
   unicoap_listener_deregister(&my_listener);
   ```

## Constrained RESTful Environments (CoRE) resource

By default, `unicoap` registers a `/.well-known/core` resource, providing a list of available
resources. @ref CONFIG_UNICOAP_WELL_KNOWN_CORE allows you to customize the default behavior.

You can also customize the link encoding per listener. By default, listeners with the
@ref unicoap_listener_t.link_encoder property unset, use the built-in link encoder.

## Request-resource matching

`unicoap` determines whether a request matches a resource in the following order. `unicoap` iterates
over all listeners and then over all resources registered with that listener. The order of listeners
follows the registration order, except the first listener is the built-in listener that handles
`/.well-known/core` and the second may be the XFA resources listener, if present. Once a matching
resource is found, no further ones are checked.

Resources that do not specify the current mode of transport in their protocol set, are not
considered. In this case, `unicoap` behaves as if the resource does not exist. To restrict a
resource to a subset of transports, set @ref unicoap_resource_t::protocols to a value produced
by @ref UNICOAP_PROTOCOLS. Please note that `unicoap` considers an unset `protocols` property
to mean "All protocols". If you want to make this explicit, use @ref UNICOAP_PROTOCOLS_ALLOW_ALL or
@ref UNICOAP_PROTOCOLS_ALLOW_NONE.

The server then checks the path of the resource. If you supplied the
@ref UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE flag, a resource registered for `/foo/bar` will also match
subpaths such as `/foo/bar/zoo` and `/foo/bar/zoo/loo`. Then, `unicoap` checks whether the request
method is in the set of allowed methods you specified. If the method does not match, and none of the
other resources indicate a match, a `Method Not Allowed` CoAP response is sent.
If no matching resource is found, a `Not Found` CoAP response is returned.

If the built-in
behavior does not fit your scenario, you can customize the matching algorithm per listener.
To do this, set the @ref unicoap_listener_t.request_matcher property. By default, if this property
is unset, the built-in matcher will be used.

## Handling requests

When registering a resource, you must specify a request handler, even if the resource's content
is statically known. To define a handler, implement a function that adheres to the signature of
@ref unicoap_request_handler_t.

### Auxiliary Information

In the handler, you are given access to an auxiliary information
object (`aux`), letting you retrieve the transport used, and both the local and client endpoint.
This may be helpful if you are using a driver able to receive messages at different addresses
(e.g., multiple sockets, ports, or multicast addresses). The `aux` also provides access to the
token, and transport-specific data, such as the message ID and type if UDP or DTLS is used.

### Responding

There are multiple techniques for responding.

1. **Return status code**: If you don't want to send a response payload and have no
   options you may want to set, you can just return a @ref unicoap_status_t. Note that this requires
   you to complete all request processing before. If this is not desirable, e.g., if you are
   performing sensitive operations that would open a timing side channel, consider using one of the
   other techniques.

   ```c
   int my_request_handler(
       unicoap_message_t* message, const unicoap_aux_t* aux,
       unicoap_request_context_t* ctx, void* arg
   ) {
       return UNICOAP_STATUS_NOT_IMPLEMENTED;
   }
   ```

2. **Call send**: Initialize a response message with the status code, and
  optionally, also payload and options. You can repurpose the memory used by the `message`
  parameter for the response, but you must not write into the payload buffer or mutate options.
  If you do want to send options, please allocate options using @ref UNICOAP_OPTIONS_ALLOC.
  Then, pass the message and the response context you were
  given as part of the handler parameters to @ref unicoap_send_response. Finally, you may inspect
  any errors that may have occurred while sending the response or continue processing the request.
  You may also directly return the result of @ref unicoap_send_response.
  Please be aware any processing performed inside this handler is executed in the server's
  processing loop and will thus block it. Given an expensive operation, we encourage you to switch
  to the third technique. Nevertheless, this should be the preferred response technique for most
  applications.

   ```c
   int my_request_handler(
       unicoap_message_t* message, const unicoap_aux_t* aux,
       unicoap_request_context_t* ctx, void* arg
   ) {
       unicoap_response_init_string(message, UNICOAP_STATUS_CONTENT, "Hello, World!");
       return unicoap_send_response(message, ctx);
   }
   ```

3. **Defer the response**: This technique is not available yet.



@remark
There is no architectural limitation in the number of sockets or ports. If
multiple sockets or ports (depending on the transport, the term _handle_ or _input_ might be more
adequate) are supported. This would be indicated on the respective driver's documentation page.


@}
