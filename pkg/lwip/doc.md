@defgroup pkg_lwip   lwIP network stack
@ingroup  pkg
@ingroup  net
@brief    Provides the lwIP network stack
@see      https://savannah.nongnu.org/projects/lwip/
@see      https://www.nongnu.org/lwip/2_1_x/

lwIP is a lightweight TCP/IP stack primarily for usage with Ethernet.
It can be used with the @ref net_sock API.

## Debugging

Debug prints can be enabled by setting the defines from the [lwIP documentation]
in the package's Makefile in `pkg/lwip/Makefile`, for example

```makefile
CFLAGS += -DLWIP_DEBUG
CFLAGS += -DUDP_DEBUG=LWIP_DBG_ON
```

[lwIP documentation](https://www.nongnu.org/lwip/2_0_x/group__lwip__opts__debugmsg.html)
