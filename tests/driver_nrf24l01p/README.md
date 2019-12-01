# About
This is a manual test application for the NRF24L01+ radio driver

For running this test, you need to connect/configure the following pins of your
radio device:
- SPI MISO
- SPI MOSI
- SPI CLK
- CS (chip select)
- CE
- IRQ (external interrupt)

You must set the following variables in make:
- SPI\_PORT:
e.g. SPI\_PORT = SPI\_DEV\\(0\\)
- CE\_PIN:
e.g. CE\_PIN   = GPIO\_PIN\\(2,6\\)
- CS\_PIN:
e.g. CS\_PIN   = GPIO\_PIN\\(0,4\\)
- IRQ\_PIN:
e.g. IRQ\_PIN  = GPIO\_PIN\\(1,8\\)

- ADDR\_P0 (address of pipe 0)
e.g. ADDR\_P0 = \\{0xe7,0xe7,0xe7,0xe7,0xe7\\}
- ADDR\_P1 (address of pipe 1)
e.g. ADDR\_P1 = \\{0xFF,0xFF,0xFF,0xFF,0xFF\\}
- ADDR\_P2 (last byte of address of pipe 2)
e.g. ADDR\_P2 = 0x04
- ADDR\_P3 (last byte of address of pipe 3)
e.g. ADDR\_P3 = 0x05
- ADDR\_P4 (last byte of address of pipe 4)
e.g. ADDR\_P4 = 0x06
- ADDR\_P5 (last byte of address of pipe 5)
e.g. ADDR\_P5 = 0x07

- PROTOCOL (whether to use ShockBurst or Enhanced Shockburst)
e.g. PROTOCOL = NRF24L01P_PROTOCOL_ESB

# Usage
For testing the radio driver you can use the
```ifconfig```, ```txtsnd```, and ```ping6``` commands.

There is a command ```nrf24ctl``` to configure an nrf24l01+ device.
Some configuration can also be done with ifconfig, but ifconfig
does not consider receiver pipes. So ifconfig only changes settings
for data pipe 0.

If you experience a significant loss of ping responses, selectively
disable calls of ```nrf24l01p_diagnostics_print_*()```, or run the
release version because it wastes a lot of time with writing to stdout.

Also with ShockBurst as a protocol you will notice lost
ping responses because ShockBurst cannot perform retransmissions.