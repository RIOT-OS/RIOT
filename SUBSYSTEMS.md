# RIOT Subsystem Maintainers

This file gives an overview about the maintainers of RIOT subsystems. Whenever
you submit an issue or a Pull Request, please assign one of the maintainers of
the affected modules as an assignee in the issue tracker. The maintainer takes
care of either fixing/merging the issue/PR herself or re-assign the issue to a
qualified developer.

**Note**: A developer might maintain more than one subsystem and an issue might
affect more than one module.

# CPU
## ESP32, ESP8266

## MSP430

- Marian Buschsieweke (@maribu)

## ARM7

- Marian Buschsieweke (@maribu)

## ARM Cortex-M

### Atmel Sam0 line (SAM D1x/D2x, L1x, L2x, D5x)

 - Benjamin Valentin (@benpicco)
 - Dylan Laduranty (@dylad)

## Atmel AVR (Atmega)

- Marian Buschsieweke (@maribu)

## Native

### ZEP radio simulation & ZEP dispatcher

 - Benjamin Valentin (@benpicco)

# Hardware modules

## Network devices
### IEEE 802.15.4

 - José I. Álamos (@jia200x)

### NRF802154

 - José I. Álamos (@jia200x)

### CC2538

 - José I. Álamos (@jia200x)

### KW2XRF

 - José I. Álamos (@jia200x)

### MRF24J40

 - José I. Álamos (@jia200x)

### AT86RF2XX

 - José I. Álamos (@jia200x)

#### AT86RF215

 - Benjamin Valentin (@benpicco)

### BLE

### LoRa

 - José I. Álamos (@jia200x)

### CC110X

- Marian Buschsieweke (@maribu)

### ESP-NOW

## Peripherals

- Marian Buschsieweke (@maribu)

## Other drivers

# Kernel (core)

  - Kaspar Schleiser (@kaspar030)

# System libraries

## High-level timer API (xtimer, ztimer)

  - Kaspar Schleiser (@kaspar030)

## Power Management

## MTD Subsystem

 - Benjamin Valentin (@benpicco)

## File Systems

## USB support

  - Dylan Laduranty (@dylad)

## POSIX support

## Scripting languages

## RUST support

  - Christian Amsüss (@chrysn)
  - Kaspar Schleiser (@kaspar030)

## C++ support

- Marian Buschsieweke (@maribu)

## Crypto

# Networking

## Network stacks
### GNRC

- Martine S. Lenders (@miri64)

#### 6LowPAN

- Martine S. Lenders (@miri64)

#### IPv6

- Martine S. Lenders (@miri64)

##### Auto-Subnetting

 - Benjamin Valentin (@benpicco)

#### Netif

 - José I. Álamos (@jia200x)

#### UDP

- Martine S. Lenders (@miri64)

### CCN-Lite

### OpenThread

 - José I. Álamos (@jia200x)

### LWIP

- Martine S. Lenders (@miri64)

### OpenWSN

 - José I. Álamos (@jia200x)

## Physical/Link Layer
### LoRaWAN

 - José I. Álamos (@jia200x)

### BLE (nimble)

### IEEE 802.15.4

 - José I. Álamos (@jia200x)

## Interfaces
### Sock

- Martine S. Lenders (@miri64)

### Netif

 - José I. Álamos (@jia200x)

### Netdev

 - José I. Álamos (@jia200x)

## Application protocols
### CoAP

#### nanoCoAP

 - Benjamin Valentin (@benpicco)

### MQTT

### LWM2M

## Others

### DTLS

- Martine S. Lenders (@miri64)

### RPL

# Build System

  - Kaspar Schleiser (@kaspar030)

# Documentation

# Testing/CI

  - Kaspar Schleiser (@kaspar030)
