/**
@defgroup    boards_remote-revb RE-Mote Revision B
@ingroup     boards
@brief       Support for the RE-Mote board Revision B

The `RE-Mote` has three versions, a first prototype A (`remote-pa`) only
distributed to beta testers, its following revision A (`remote-reva`), and the
latest revision B (`remote-revb`) which are commercially available. The
following section focuses on the revision B.

The official RE-Mote wiki page is maintained by Zolertia at:

https://github.com/Zolertia/Resources/wiki/RE-Mote

# Components

| MCU   | [CC2538 (ARM Cortex-M3 with on-board 2.4GHz radio)](http://www.ti.com/product/CC2538) |
|-------|-----------------------------------------------------------------------------------------------------|
| Radio | Two radio interfaces (IEEE 802.15.4): [2.4GHz](http://www.ti.com/product/CC2538) and [863-950MHz](http://www.ti.com/product/CC1200) |
|  | RP-SMA connector for external antenna (with a RF switch to select either 2.4GHz/Sub-GHz radio)           |
| USB-to-Serial | [CP2104](https://www.silabs.com/documents/public/data-sheets/cp2104.pdf) |
| Peripherals | RTCC, built-in battery charger for LiPo batteries, External WDT (optional), Micro-SD |
| Others | RGB LED, power management block (150nA when the mote is shutdown)|

# Porting status

In terms of hardware support, the following drivers have been implemented
for CC2538 System-on-Chip:

- UART
- Random number generator
- Low Power Modes
- General-Purpose Timers
- I2C/SPI library
- LEDs
- Buttons
- RF 2.4GHz built-in in CC2538
- RF switch to programmatically drive either 2.4GHz or sub-1GHz to a single
  RP-SMA

And under work or pending at CC2538 base CPU:

- Built-in core temperature and battery sensor.
- CC1200 sub-1GHz radio interface.
- Micro-SD external storage.
- ADC
- USB (in CDC-ACM).
- uDMA Controller.

# Layout

![layout](http://i.imgur.com/4bV6lyYl.png)

# Flashing

The RE-Mote has built-in support to flash over USB using the BSL.  Previous
versions required to unlock the bootloader by manually pressing the `user
button` and `reset button`, but the current version handles the sequence with an
on-board PIC, so automatically unlocks the bootloader upon flashing.

e.g.
```
Bash
$ make BOARD=remote-reva flash
```

The RE-Mote Revision B has the following pin-out:

![RE-Mote pin-out](https://i.imgur.com/PpWzzRJ.png)

# Pin out and connectors

## RE-Mote ports and connectors

![](http://i.imgur.com/TF21Hin.png)

![](http://i.imgur.com/J7aisKJ.png)

## RE-Mote on-board connectors pin-out

The RE-Mote uses the [Molex 5-pin WM4903-ND male header connector](http://datasheets.globalspec.com/ds/5843/DigiKey/6D12815C-098E-40A3-86A0-22A3C50B75BA) to
connect digital sensors based on I2C and SPI protocols, as well as other sensors
or actuators you may need to connect.  The pins are 2.54 mm spaced and the
connector has the following pin-out:

![](http://i.imgur.com/2DZ17PV.png)
![](http://i.imgur.com/q7Hb7Y8.png)
 */
