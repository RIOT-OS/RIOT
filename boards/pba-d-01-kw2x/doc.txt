/**
@defgroup    boards_pba-d-01-kw2x phyWAVE-KW22 Board
@ingroup     boards
@brief       Support for the phyWAVE evaluation board

## Manufacturer
Designed and produced by PHYTEC Messtechnik GmbH, D-55129 Mainz.
[Homepage](http://www.phytec.de) | Ordering information via mail at
contact@phytec.de

## Overview

The phyNODE evaluation board consists of a [phyWAVE](https://www.phytec.de/produkte/internet-of-things/phywave/)
module mounted on a common PCB with several sensor and actuator modules.
The board provides 2 USB ports. The one next to the white imprint *cmsis-dap*
is meant for development (debug and flash). The second one, next to the golden
*PHYTEC* imprint, is connected to the USB peripheral of the MCU and may be used
by a firmware (currently not supported).


The implementation status of the phyWAVE and phyNODE peripherals are listed
below. You can find links to the corresponding Pull Requests which may give you
a code reference.

<img src="https://raw.githubusercontent.com/jremmert-phytec-iot/Pictures_Phytec_IOT/master/EvalBoard_3.png" width="800" />
phyNODE board with equipped phyWAVE-KW2x processor/radio-module.

@note The PCB-pieces with the mounted sensors can be separated from the
      evaluation board to capture sensor values from specific spatial points.

## Pinout Reference

The board features an Arduino like header connector to mount shields. The
spacing of the 4 pin rows is a bit offset, thus shields might not fit perfectly.

The pinout below shows the vendor specific naming and functionality.
The actual pin configuration of the board for RIOT can be found in
[periph_conf.h](https://github.com/RIOT-OS/RIOT/blob/master/boards/pba-d-01-kw2x/include/periph_conf.h).

@note Not all signals and pins are compatible with Arduino Uno R3.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                             _X1____________________
                            |                   D15 |___DIO21, PE19, I2C0 SCL
                            |                   D14 |___DIO20, PE18, I2C0 SDA
                            |                  AREF |___AREF,  VCC3V3
                            |                  GND3 |___GND
                        X___| RESERVED          D13 |___DIO3,  PC5,  SPI0 SCK
                   VCC3V3___| IOREF             D12 |___DIO5,  PC7,  SPI0 MISO
                   nRESET___| RST TGTMCU        D11 |___DIO4,  PC6,  SPI0 MOSI
                   VCC3V3___| P3V3              D10 |___DIO2,  PC4,  SPI0 CS0
              P5V USB OUT___| P5V USB            D9 |___DIO9,  PD4,  PWM
                GND_________| GND2               D8 |___DIO6,  PD1
                        \___| GND1                  |
                        X___| P5_9V VIN          D7 |___DIO24, PA1,  PWM,  TDI2
                            |                    D6 |___DIO25, PA2,  PWM,  TDO2
 DIO15, PE2, ADC         ___| A0                 D5 |___DIO11, PD6,  PWM
 DIO16, PE3, ADC, TDO1   ___| A1                 D4 |___DIO29, PA19, TCLKIN
 DIO12, PD7, ADC         ___| A2                 D3 |___DIO27, PA4,  PWM
 DIO10, PD5, ADC         ___| A3                 D2 |___DIO17, PE4,  LLWU, TDI1
 DIO13, PE0, ADC, I2CSDA ___| A4                 D1 |___DIO8,  PD3,  UART2 TX
 DIO14, PE1, ADC, I2CSCL ___| A5                 D0 |___DIO7,  PD2,  UART2 RX
                             -----------------------
                             Arduino Uno R3 Connector
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | [MKW22D512](http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=KW2x) | yes | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265) |
| phyWAVE board support | [phyWAVE](http://www.phytec.de/de/produkte/internet-of-things/phywave.html) | yes | [mainline](https://github.com/RIOT-OS/RIOT/pull/2059) |
| Low-level driver | GPIO    | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265)|
|        | PWM       | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265)|
|        | UART      | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265)|
|        | I2C       | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265)|
|        | SPI       | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265) |
|        | USB-Device    | yes       | [WIP](https://github.com/RIOT-OS/RIOT/pull/3890)|
|        | RTT       | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265) |
|        | RNG       | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265) |
|        | Timer     | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2265) |
| Radio Chip | integrated    | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2756) |
| Humidity Sensor    | [HDC1000](http://www.ti.com/lit/ds/symlink/hdc1000.pdf)   | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2070) |
| Pressure Sensor    | [MPL3115A2](http://www.nxp.com/products/sensors/pressure-sensors/barometric-pressure-15-to-115-kpa/20-to-110kpa-absolute-digital-pressure-sensor:MPL3115A2?) | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2123) |
| Tri-axis Accelerometer | [MMA8652FC](http://www.nxp.com/products/sensors/accelerometers/3-axis-accelerometers/2g-4g-8g-low-g-12-bit-digital-accelerometer:MMA8652FC)   | yes | [mainline](https://github.com/RIOT-OS/RIOT/pull/2119) |
| Magnetometer   | [MAG3110FCR1](http://www.nxp.com/products/sensors/magnetometers/sample-data-sets-for-inertial-and-magnetic-sensors/freescale-high-accuracy-3d-magnetometer:MAG3110)   | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2121) |
| Light Sensor   | [TCS3772](https://ams.com/documents/20143/36005/TCS3772_DS000175_3-00.pdf/8689a345-f46b-d3f0-f839-eb8d38ead80d)   | yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/3135) |
| IR-Termopile Sensor    | [TMP006](http://www.ti.com/ww/eu/sensampbook/tmp006.pdf)    |yes       | [mainline](https://github.com/RIOT-OS/RIOT/pull/2148) |
| Capacitive Button  | PCB   | yes        | [mainline](https://github.com/RIOT-OS/RIOT/pull/7147) |
 */
