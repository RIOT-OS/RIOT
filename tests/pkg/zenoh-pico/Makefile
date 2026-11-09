include ../Makefile.pkg_common

USEPKG += zenoh-pico
USEMODULE += embunit
USEMODULE += ztimer_msec

# the sessions communicate over the IPv6 loopback address, no network
# interface is needed
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_tcp
USEMODULE += gnrc_tcp

# peer mode is needed to test without a zenoh router, it must be set for the
# package and the application
CFLAGS += -DZ_FEATURE_UNICAST_PEER=1
# one receive buffer per connection: listener, accepted and connector socks
CFLAGS += -DCONFIG_GNRC_TCP_RCV_BUFFERS=3
# don't wait 2 x 30 s in TIME_WAIT when closing the connections
CFLAGS += -DCONFIG_GNRC_TCP_MSL_MS=100
# closing the listening session first waits for the end of the accepted
# connection, until the connection timeout (120 s by default) as the
# connecting session closes its side when it is closed itself
CFLAGS += -DCONFIG_GNRC_TCP_CONNECTION_TIMEOUT_DURATION_MS=2000

# z_open() runs on the stack of the main thread: 3 KiB more than the
# default (1.5 KiB on Cortex-M)
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(THREAD_STACKSIZE_DEFAULT+3072\)

include $(RIOTBASE)/Makefile.include
