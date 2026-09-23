@defgroup pkg_picohttpparser  Tiny HTTP/1.x request and response parser
@ingroup  pkg
@brief    Provides a minimal, dependency-free HTTP/1.x request parser

[PicoHTTPParser](https://github.com/h2o/picohttpparser) is a tiny, primitive,
fast HTTP request/response parser.

It only parses HTTP/1.x requests and responses. It is neither a webserver, nor
an HTTP client. It does not open sockets, allocate buffer bodies or build
responses. Unlike most parsers, it is stateless and does not allocate memory by
itself. All it does is accept a pointer to the buffer and the output structure,
and sets up the pointers in the latter to point at the necessary portions of
the buffer.

## Usage
The parser must be added to the `Makefile` with

```makefile
USEPKG += picohttpparser
```

Then, include the header file `picohttpparser.h` and start using the
`phr_parse_request` or `phr_parse_response` functions. Both implementations do
not copy the data, they only set pointers to the data in the buffer, so the
buffer must remain valid during processing.

## As a request parser

Below is a minimal example of how to use the parser to parse an HTTP request
that is stored in a buffer `request`.

```c
#define MAX_HEADERS 8

const char *method;
size_t method_len;
const char *path;
size_t path_len;
int minor_version;
struct phr_header headers[MAX_HEADERS];
size_t num_headers = MAX_HEADERS;

int res = phr_parse_request(request, sizeof(request) - 1, &method, &method_len,
                            &path, &path_len, &minor_version, headers,
                            &num_headers, 0);

if (res == -2) {
    /* request is incomplete, need more data */
}
else if (res == -1) {
    /* request is invalid */
}
else {
    /* request is valid, res points to the start of the body (if any) */
}
```

## As a response parser

Below is a minimal example of how to use the parser to parse an HTTP response
that is stored in a buffer `response`.

```c
#define MAX_HEADERS 8

int minor_version;
int status;
const char *msg;
size_t msg_len;
struct phr_header headers[MAX_HEADERS];
size_t num_headers = MAX_HEADERS;

int res = phr_parse_response(response, sizeof(response) - 1, &minor_version,
                             &status, &msg, &msg_len, headers, &num_headers, 0);

if (res == -2) {
    /* response is incomplete, need more data */
}
else if (res == -1) {
    /* response is invalid */
}
else {
    /* response is valid, res points to the start of the body (if any) */
}
```
