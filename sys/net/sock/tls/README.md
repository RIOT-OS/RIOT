# TLS Socket API for RIOT OS

Implementation of TLS over TCP socket using wolfSSL for RIOT OS. This module provides a secure communication layer that operates independently from the underlying network stack.

## Overview

This TLS implementation provides:
- Server authentication using certificates
- Support for TLS 1.2
- Integration with RIOT's sock TCP API
- Based on wolfSSL as the TLS backend
- Configurable chunk sizes for optimal memory usage
- Timeout handling and automatic retries

## Features

- **Server Authentication only**: Servers authenticate themselves to clients using certificates signed by a self-signed CA
- **Memory Efficiency**: Configurable chunk-based read/write operations
- **Error Handling**: Comprehensive error handling with specific error codes
- **Timeout Management**: Configurable timeouts with automatic retry mechanism
- **Clean API**: Simple and intuitive API following RIOT OS conventions

## Requirements

The following modules are required and automatically included when using this module:

```makefile
USEMODULE += tcp_tls
```

This will pull in all necessary dependencies including:
- Network stack modules (GNRC, netdev)
- TCP socket support
- WolfSSL and its cryptographic modules
- Timer modules

## Configuration

The following configuration options are available:

```c
/* Maximum chunk size for read/write operations */
#define CHUNK_SIZE 256

/* Default timeout value in milliseconds */
#define TLS_DEFAULT_TIMEOUT 5000

/* Maximum number of retry attempts */
#define MAX_TRIES 3
```

## API

### Basic Usage

1. **Creating a TLS Socket**:
```c
#include "net/sock/tls.h"

sock_tls_tcp_t sock;
int ret = sock_tls_tcp_create(&sock, wolfTLSv1_2_client_method());
```

2. **Connecting to a Server**:
```c
sock_tcp_ep_t remote = {
    .family = AF_INET6,
    .port = SERVER_PORT,
};
ret = sock_tls_tcp_connect(&sock, &remote, 0, 0);
```

3. **Reading Data**:
```c
char buffer[256];
ssize_t received = sock_tls_tcp_read(&sock, buffer, sizeof(buffer));
```

4. **Writing Data**:
```c
const char *message = "Hello, TLS!";
ssize_t sent = sock_tls_tcp_write(&sock, message, strlen(message));
```

5. **Cleanup**:
```c
sock_tls_tcp_disconnect(&sock);
```

### Server Implementation

1. **Creating a Server**:
```c
sock_tls_tcp_queue_t queue;
sock_tcp_ep_t local = {
    .family = AF_INET6,
    .port = SERVER_PORT,
};
sock_tcp_t queue_array[1];

ret = sock_tls_tcp_listen(&queue, &local, queue_array, 1, 0,
                         wolfTLSv1_2_server_method(),
                         server_cert, server_cert_len,
                         server_key, server_key_len);
```

2. **Accepting Connections**:
```c
sock_tls_tcp_t *client_sock;
ret = sock_tls_tcp_accept(&queue, &client_sock, ACCEPT_TIMEOUT);
```

## Error Handling

The API functions return negative errno values in case of errors:
- `-EINVAL`: Invalid parameters
- `-ENOMEM`: Out of memory
- `-ECONNRESET`: Connection reset by peer
- `-ETIMEDOUT`: Operation timed out

## Example

Here's a minimal client example:

```c
#include "net/sock/tls.h"

int tls_client_example(void) {
    sock_tls_tcp_t sock;

    /* Create TLS socket */
    if (sock_tls_tcp_create(&sock, wolfTLSv1_2_client_method()) < 0) {
        return -1;
    }

    /* Connect to server */
    sock_tcp_ep_t remote = {
        .family = AF_INET6,
        .port = SERVER_PORT,
    };
    if (sock_tls_tcp_connect(&sock, &remote, 0, 0) < 0) {
        return -1;
    }

    /* Send data */
    const char *msg = "Hello, TLS!";
    ssize_t sent = sock_tls_tcp_write(&sock, msg, strlen(msg));

    /* Cleanup */
    sock_tls_tcp_disconnect(&sock);
    return (sent > 0) ? 0 : -1;
}
```

## Contributing

This module is maintained by Bilal-Alali. For bugs, feature requests, or questions, please open an issue on the RIOT OS GitHub repository or send me an email alalibilal632@gmail.com

## License

This module is licensed under the terms of the GNU Lesser General Public License v2.1.

## Author

- **Bilal-Alali**
- Created: 2025-05-23
