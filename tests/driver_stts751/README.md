# STTS751 I2C Temperature Sensor

## Overview

This test application demonstrates the usage of the STTS751 driver interface
and can be used to test a STTS751 device with shell commands.

## Wiring

Only 4 wires are needed:
- GND
- VCC
- SDA
- SCL

On the CoreCell SX1302:

| Nucleo    | CN1 |
|-----------|-----|
| GND       | 39  |
| 3V3       | 1   |
| 5V        | 2   |
| SDA (D14) | 3   |
| SCL (D15) | 5   |

with CN1 being the 40 pins connector, starting with 1 in top right:  
1 3 5 ...  
2 4 6 ...  

## Usage

The following shell commands are available:
* **temp:** Get the temperature (Celcius)