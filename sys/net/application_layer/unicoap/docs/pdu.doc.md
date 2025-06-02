@defgroup net_unicoap_pdu Parsing and Serialization
@ingroup net_unicoap_message
@brief Tools for parsing PDUs and serializing messages and options

## Parsing
To parse a message on your own, use one of the parsers in @ref net_unicoap_message and
@ref unicoap_parser_result_t. The parsed message structure helps you allocate everything needed in
one go.

```c
// Parse an RFC 7252 PDU
uint8_t pdu[] = {
    0x40, 0x02, 0xfe, 0xb1, 0xb9, 0x61, 0x63, 0x74, 0x75, 0x61, 0x74, 0x6f, 0x72, 0x73, 0x04, 0x6c, 0x65, 0x64, 0x73, 0x11, 0x32, 0x37, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3d, 0x67, 0x21, 0x32, 0xff, 0x6d, 0x6f, 0x64, 0x65, 0x3d, 0x6f, 0x6e
};

unicoap_parser_result_t parsed = { 0 }; // Zero-initialize everything

// Parse message
ssize_t res = unicoap_pdu_parse_rfc7252_result(pdu, sizeof(pdu), &parsed);

// Handle errors
if (res < 0) {
    // eat error
}

// parsed.message contains the parsed message
```

## Serializing
To serialize a message on your own, decide whether you need vectored data or contiguous data.
To get vectored data, you use `unicoap_pdu_buildv_*` functions, depending on the CoAP transport you intend to use.
Vectored data is represented using an @ref iolist_t.
If you want to build a contiguous storage body, refer to the `unicoap_pdu_build_*` methods, also depending
on the transport.

```c
// Build an RFC 7252 PDU
uint8_t pdu[MY_CAPACITY] = { 0 };
ssize_t size = unicoap_pdu_build_rfc7252(pdu, sizeof(pdu), message, properties);

// Handle errors
if (res < 0) {
    // eat error
}
```

## Implementation
Parsing and serialization are each done in two steps:
1. Message header up until options is parsed/serialized by driver
2. Options are parsed/serialized by common `unicoap` implementation
