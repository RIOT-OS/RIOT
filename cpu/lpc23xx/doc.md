@defgroup   cpu_lpc23xx NXP LPC23XX
@ingroup    cpu
@brief      NXP LPC23XX family specific code

## Overview

NXP LPC23XX family specific code

## First Steps

- See the manual for toolchain and ide setup
- <a href="http://www.nxp.com/pip/LPC2387_2.html">Documentation</a>

## Components

- \ref cpu_arm7_common

## USB device

The USB device driver supports full-speed control, bulk, and interrupt transfers
using the controller's slave (CPU-driven) mode. Isochronous transfers and DMA
are not implemented. The driver uses USB port 1 on parts with two ports.

The board must provide a crystal and connect D+ (P0.29), D- (P0.30), VBUS
(P1.30), and the external pull-up switching circuit (USB_CONNECT, P2.9).
The USB clock is derived from the CPU's 288 MHz PLL output.

Endpoints have fixed types: endpoint 0 is control, endpoints 1/4/7/10/13 are
interrupt, and endpoints 2/5/8/11/14/15 are bulk. Allocation is limited by
`USBDEV_NUM_ENDPOINTS` (8 by default, at most 16).
