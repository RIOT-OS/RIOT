# Testing rapidly sending packets while the receiving node tries to reply

## Usage

This test is supposed to be used in tandem with [netdev_flood_replier].

**Before** you start the replier first flash this application to another board
that supports the same link-layer technology as the device under test on the
replier:

```sh
make flash
```

You can test if everything went right by connecting via terminal to the node

```sh
make term
```

If after a reset the following message is shown and no error messages follow,
the device started flooding successfully

```
Starting to flood now; start an instance of tests/netdev_flood_replier
(on a device with matching link-layer ;-)) to see results.
```

## Support for more devices
Currently the following devices are supported with this test:

- `at86rf2xx`

To extend the coverage of this test, just add the setup and sending behavior for
your device in the functions it is documented

```C
/**
 * @note    Please amend for more device support
 */
```

[netdev_flood_replier]: ../netdev_flood_replier/
