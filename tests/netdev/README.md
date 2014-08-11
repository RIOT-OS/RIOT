# netdev tests
This application tests the module `netdev`, it's base module `netdev_base` in
particular.
The tests can be automated by running

```bash
SENDER=0 make all flash && SENDER=1 make all flash && make test
```

The values of the network addresses, the channel, and the network ID can be
configured by the environment variables

* `NETDEV_TEST_RECEIVER` and `NETDEV_TEST_SENDER`,
* `NETDEV_TEST_CHANNEL`,
* `NETDEV_TEST_NID`

You can set the role of the application by setting the `SENDER` variable to
either 0 (application is receiver) or any other value (applictation is sender),
but keep in mind, that both versions have to be flashed to to seperate devices
(identified by `RECEIVER_PORT` and `SENDER_PORT` respectively) in order for
`make test` to succeed.
