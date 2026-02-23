@defgroup net_unicoap_drivers_dtls CoAP over DTLS Driver
@ingroup net_unicoap_drivers
@brief Use CoAP over DTLS, with optional reliability
@{

Module. Specify `USEMODULE += unicoap_driver_dtls` in your application's Makefile.

Include these headers required for managing DTLS credentials.
```c
#include "net/sock/dtls/creds.h"
#include "net/credman.h"
#include "net/dsm.h"
```

Then, in your application, call @ref sock_dtls_add_credential to add a DTLS credential. If you need
to access the DTLS socket, call @ref unicoap_transport_dtls_get_socket(). You must add
DTLS credentials yourself. To change the UDP port `unicoap` is listening on,
modify @ref CONFIG_UNICOAP_DTLS_PORT. The CoAP over DTLS driver uses the same socket both for
client and server functionality.

@see @ref UNICOAP_PROTO_DTLS
@see @ref unicoap_rfc7252_message_type_t

This is the dependency graph of this driver:

```
unicoap_driver_dtls
├── unicoap_driver_rfc7252_common
│   ├── unicoap_driver_rfc7252_common_messaging
│   └── unicoap_driver_rfc7252_common_pdu
├── unicoap_sock_support
│   ├── sock_async
│   ├── sock_async_event
│   ├── sock_aux_local
│   └── sock_util
├── ... (operating system networking modules)
.
```

@}
