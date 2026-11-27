# zenoh-pico

Unit tests of the zenoh-pico package. Two zenoh sessions in peer mode
communicate over TCP on the IPv6 loopback address (`tcp/[::1]:7447`) of GNRC:
one session listens, the other connects to it. No network interface and no
zenoh router are needed.

The tests check:

- the elapsed time functions of the clock of the RIOT platform layer,
- the opening of the two sessions,
- a publication received by a subscriber of the other session,
- a query answered by a queryable of the other session,
- closing the listening session first, then opening the sessions again and
  publishing,
- closing the sessions.

## Usage

```
make BOARD=native64 all test
```

The application can also run on a board, with the same command.

The test configures GNRC TCP for short tests (see the `Makefile`): one receive
buffer per connection, a short TIME_WAIT and a short connection timeout, so
that the sessions can be closed and opened again quickly.

For a zenoh application that connects to a zenoh router, see the
[zenoh-pico example](../../../examples/networking/misc/zenoh-pico/README.md).
