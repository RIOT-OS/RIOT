@defgroup net_unicoap_message_example Using Message APIs
@ingroup  net_unicoap_message
@brief    A demo of `unicoap` message APIs
@{

Sample code.
This example demonstrates how you can use `unicoap` message and options APIs and how to parse PDUs.
You can find a demo application in the `examples/networking/coap/unicoap_message` folder.

## Bytes to Message (Deserializing)

### Parsing a PDU

To start, let us assume `pdu` is a buffer containing the CoAP PDU.
```c
const uint8_t pdu[] = { /* ... */ };
```

Next, allocate a result structure.

```c
unicoap_parser_result_t parsed = { 0 };
```

Then, call one of the message parsers. CoAP supports different transports which is why the CoAP PDU
header varies. In this case, let us assume we received the message over UDP or DTLS. In these cases,
we use the RFC 7252 PDU format. Using the result structure frees you of needing to allocate options
and a message struct and to wire up options with message struct.

```c
if ((res = unicoap_pdu_parse_rfc7252_result(pdu, sizeof(pdu), &parsed)) < 0) {
    puts("Error: parsing failed");
    return;
}

unicoap_message_t* message = &parsed.message;
```

Because the header varies, transport-dependent details like the RFC 7252 message type and ID
are accessible via the
@ref unicoap_message_properties_t::rfc7252 member.

```c
printf("CoAP message has token=<%i bytes>\n",
       parsed.properties.token_length);

printf("CoAP over UDP/DTLS has id=%i type=%s\n",
       parsed.properties.rfc7252.id,
       unicoap_string_from_rfc7252_type(parsed.properties.rfc7252.type));
```

### Inspecting a Message

You use the
@ref unicoap_message_is_request,
@ref unicoap_message_is_response, and
@ref unicoap_message_is_signal
methods to check whether a given message is a request, response, or signaling message.

The corresponding typed view of the code is accessible through
@ref unicoap_message_t.method,
@ref unicoap_message_t.status, and
@ref unicoap_message_t.signal.

You can also obtain a human-readable constant null-terminated C string. There are also versions
available for status codes and signal numbers. To get a string description of the CoAP code
without checking the message class first, use @ref unicoap_string_from_code.

```c
const char* method_name = unicoap_string_from_method(message->method);
```

The payload and payload size in bytes can be retrieved the
@ref unicoap_message_t.payload and
@ref unicoap_message_t.payload_size members.

### Reading Options

First, let us dump all options to the standard output.

```c
unicoap_options_dump_all(message->options);
```

To read options like `Content-Format` which can occur no more than once, you use
@ref unicoap_options_t::unicoap_options_get_content_format.
Read accessors for non-repeatable options are prefixed with `unicoap_options_get`.

```c
unicoap_content_format_t format = 0;

if (unicoap_options_get_content_format(message->options, &format) < 0) {
    puts("Error: could not read Content-Format!");
}

assert(format == UNICOAP_FORMAT_JSON);
```

Options like `Uri-Query` can occur more than once. For these types of options, `unicoap` defines
several convenience accessors. Let us retrieve the first `Uri-Query` option.

```c
const char* query = NULL;

ssize_t res = unicoap_options_get_first_uri_query(message->options, &query);
if (res < 0) {
    if (res == -ENOENT) {
        puts("Message has no Uri-Query option");
    }
    printf("Error: could read first Uri-Query option");
}
```

The `first` getter provides a view into the PDU buffer. The returned string
is thus not null-terminated.

```c
printf("First URI query: '%.*s'\n", (int)res, query);
```

In the case of URI queries, you can also retrieve queries by name (if they obey the `name=value`
format).

```c
res = unicoap_options_get_first_uri_query_by_name(message->options, "color", &query);
if (res < 0) {
    /* The getter also fails in cases where no option was found */
    if (res == -ENOENT) {
        puts("Message has no 'color' query");
    }
    printf("Error: could read first 'color' query");
}
```

For a number of repeatable options, such as `Uri-Path`, `Location-Path`, `Uri-Query`,
and `Location-Query`, `unicoap` offers accessors that generate the original, contiguous representation.
This means that multiple `Uri-Path` options are stitched back together, forming the `/original/path`.
These accessores do copy. Now, let us create a query string (`?a=1&b=2&c=3`).

```c
char query_string[50] = { 0 };

res = unicoap_options_copy_uri_queries(message->options, query_string, sizeof(query_string));
if (res < 0) {
    puts("Error: could not generate URI query string");
}
```

Alternatively, you can iterate over all query options, avoiding the copy operation and allocation.
To do this, you will need to allocate an
@ref unicoap_options_iterator_t and initialize it using
@ref unicoap_options_iterator_t::unicoap_options_iterator_init.
This is the main tool to iterate over options.
`unicoap` exposes multiple methods for getting the next instance of a repeatable option.

```c
unicoap_options_iterator_t iterator;
unicoap_options_iterator_init(&iterator, message->options);

while ((res = unicoap_options_get_next_uri_query(&iterator, &query)) >= 0) {
    printf("- URI query: '%.*s'\n", (int)res, query);
}
```

The option iterator can also be used to iterate over all options, regardless of their type.

```c
unicoap_options_iterator_init(&iterator, message->options);
unicoap_option_number_t number;
const uint8_t* value = NULL;

while ((res = unicoap_options_get_next(&iterator, &number, &value)) >= 0) {
    const char* name = unicoap_string_from_option_number(number);
    printf("- option %s nr=%i contains %" PRIuSIZE " bytes\n", name, number, res);
}
```

## Message to Bytes (Serializing)

### Creating a Message Container

Since we want to add options to the CoAP message, we need to allocate an options buffer first.
To avoid the boilerplate necessary for allocating a helper structure and buffer and the initialization
work, you just need to call
@ref UNICOAP_OPTIONS_ALLOC and provide the desired buffer capacity.

```c
UNICOAP_OPTIONS_ALLOC(options, 100);
```

Now, let us initialize a message. You can either use the designated initializer or initializer
function.

```c
unicoap_request_init_string_with_options(&message, UNICOAP_METHOD_POST, "Hello, World!", &options);
```

### Customizing Options

To set non-repeatable options like `Content-Format`, use `unicoap_options_set` accessors.

```c
int res = unicoap_options_set_content_format(&options, UNICOAP_FORMAT_TEXT);
if (res < 0) {
    puts("Error: could not set Content-Format");
}
```

For repeatable options, `unicoap` provides two versions. You can either add multiple instances
of an option like `Uri-Path` by providing the original, contiguous representation (e.g., the path).

```c
int res = unicoap_options_add_uri_path_string(&options, "/thermostat/temperature");
if (res < 0) {
    if (res == -ENOBUFS) {
        puts("Error: options buffer too small");
    }
    puts("Error: could not add URI path");
}
```

Or, you can add _components_ individually as follows.

```c
res = unicoap_options_add_uri_path_component_string(&options, "thermostat");
if (res < 0) {
    puts("Error: could not add path component");
}
res = unicoap_options_add_uri_path_component_string(&options, "temperature");
if (res < 0) {
    puts("Error: could not add path component");
}
```

The same applies to `Uri-Query`.

```c
res = unicoap_options_add_uri_queries_string(&options, "unit=C&friendly=yes");
if (res < 0) {
    puts("Error: could not add URI query");
}
```

`unicoap` offers versions for both null-terminated C strings and strings without a null-terminator
that require a length indication instead. Example:
@ref unicoap_options_t::unicoap_options_add_uri_queries and
@ref unicoap_options_t::unicoap_options_add_uri_queries_string, or
@ref unicoap_options_t::unicoap_options_add_uri_query and
@ref unicoap_options_t::unicoap_options_add_uri_query_string.

### Serializing a Message

First, allocate a buffer with a capacity of your choice.

```c
uint8_t pdu[200];
```

The header format varies depending on the transport. Let's use CoAP over UDP or CoAP over DTLS,
i.e., the RFC 7252 format.

@remark In this very simple scenario, we don't use a token. Very constrained nodes
are allowed to handle one request at a time and thus don't need a token to differentiate
responses to outstanding requests.

```c
unicoap_message_properties_t properties = {
    .token = NULL,
    .token_length = 0,
    .rfc7252 = {
        .id = 0xABCD,
        .type = UNICOAP_TYPE_NON
    }
};
```

Finally, call the serializer appropriate for the transport.

```c
ssize_t res = unicoap_pdu_build_rfc7252(pdu, sizeof(pdu), message, &properties);
if (res < 0) {
    if (res == -ENOBUFS) {
        puts("Error: PDU buffer too small");
    }
    puts("Error: could not serialize message");
    return;
}

printf("The final PDU has a size of %" PRIuSIZE " bytes.\n", res);
```

@}
