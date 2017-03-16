# About
This is a manual test application for testing the KW2XRF device driver. It enables several shell commands, allowing for special test modes of that device. Without deeper knowledge about the hardware, this application might be useless except for compile tests. If you want an example to use this radio, check the [examples](../../examples) folder.

For running this test, you need to connect/configure the following pins of your
radio device. If you are running on a phyNODE, this is already the on-board default.

- SPI DEV
- CS (chip select)
- INT (external interrupt)
