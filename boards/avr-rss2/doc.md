@defgroup    boards_avr-rss2 Atmega256RFR2 rss2
@ingroup     boards
@brief       Support for the Radio Sensors Atmega256RFR2 rss2 board

### General information
Document version V1.4 2019-11-10<br>
Board is based AtMega256RFR2 with additional HW, EUI64 chip, HW comparator, RTC XTAL, BME280

- Directory `$(RIOTBASE)/boards/avr-rss2`

### Board Features

- PCB Antenna. Supercardiod
- Robust radio performance. Up to 300 meter line-of-sight
- Unique EUI64 address via 24MAC602 chip with 2k EEPROM
- Rev 2.4 Onboard BME280 for Temp/RH/Pressure
- Light Sensor
- 32kHz RTC clock xtal
- HW comparator chip and input
- Programmable power FET, (relay) for power on/off of sensors
- Up to 25V DC input via LDO (LP2950). TVS protected
- Standard. 6-pin TTL-USB header compatible w. FTDI cable for USART
- PCB formfactor for cheap project box G.40X IP54
- Power/current use:
*- RX ~10mA (Full RPC AtMegaXXRFR2)
*- Sleep ~45uA @ 16MHz XTAL
*- Sleep ~15uA @  8MHz using internal oscillator
- Preprogammed Atmel standard stk500v2 bootloader
- CE certified by test institute

### USART
The board has one USART via the 6-pin TTL-USB adapter, The default
baudrate is 115200 bps. It's possible to use higher speeds as is 250k
and 500k baud which gives 0% Error with 16MHz clock.
(An additional USART is on the chip but as default used by interrupt
pins. Needs HW mod to be used)

### Toolchain
The Atmel toolchain is available in most operating systems.
For a full toolchain and easy installation on Ubuntu:
```
apt-get install gcc-avr avr-libc avrdude
```
Otherwise if OS toolchain packages do not support the AtMega256RfR2
MCU, another option is to download the C compiler toolchain from Microchip.

### For Linux and MacOS
1. Download the proper 8-bit C compiler AVR toolchain, 32 or 64-bit, [from Microchip](https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers).
2. Unpack under `/usr/local`
3. Add to your search PATH. For example add to `.bashrc`:
```
export PATH=$PATH:/usr/local/avr8-gnu-toolchain-linux_x86_64/bin (64-bit)
export PATH=$PATH:/usr/local/avr8-gnu-toolchain-linux_x86/bin (32-bit)
```
4. For flash programming, you need `avrdude`. On Ubuntu Linux, it can be installed with the command

```
apt-get install avrdude
```
For MacOS, there is a [HomeBrewpackage](https://formulae.brew.sh/formula/avrdude#default)
that can be installed with the command:
```
brew install avrdude
```

### RIOT build

```
make BOARD=avr-rss2
```

### Flashing/Programming hardware
Programming using avrdude using serial bootloader using a TTL-USB cable.
Press the RESET button. The bootloader with wait for boot commands for 3 seconds. Program with:
```
avrdude -p m256rfr2 -c stk500v2 -P /dev/ttyUSB0 -b 115200 -e -U flash:w:bin/rss2-mega256rfr2/hello-world.elf
```

### Board approvals

Summary: CE approved Radio Equipment Directive (RED) 2014/53/EU
Rev 2.4
- Safety: IEC 60950-1:2005 2nd Edition +Am 1:2009 +Am 2:2013
- RF: ETSI EN 300 328 V2.1.1 (2016-11)
- EMC: Draft ETSI EN 301 489-1 V2.2.0 (2017-03),Draft ETSI EN 301 489-17 V3.2.0 (2017-03)
- EMF: EN 62479:2010
- Human exposure to electromagnetic fields: EN 62479:2010

### Commercial availability
Through vendor and though resellers. Note board is will only available
were CE approval is covered. This may be further restricted by WEEE.
Contact vendor. For research legislation is more relaxed in most countries.

### References
- AtMega64/128/256/RFR2 chip documentation available via Atmel.
- Smart Reduced Power Consumption Techniques. AT02594 available via Atmel.
- Board documentation, schematics, CE and vendor via http://radio-sensors.com/

### Board (Rev2.4) anatomy with connectors
- http://radio-sensors.com/pictures/s2-2.4-front-edited.jpg
- http://radio-sensors.com/pictures/s2-2.4-back-port-edited.jpg