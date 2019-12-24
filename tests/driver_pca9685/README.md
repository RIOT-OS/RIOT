# PCA9685 I2C PWM controller

## Overview

This test appliation demonstrates the usage of the PCA9685 driver interface
and can be used to test each PCA9685 PWM device with shell commands.

The application bases on the test application for PWM peripheral drivers
which is under following copyright:

     Copyright (C) 2014-2015 Freie Universität Berlin
     @author Hauke Petersen <hauke.petersen@fu-berlin.de>
     @author Semjon Kerner <semjon.kerner@fu-berlin.de>

## Usage

To use the test application, compile it with
```
     make -C tests/driver_pca9685 BOARD=...
```
Please check the default configuration parameters in
`$(RIOTBASE)/drivers/pca9685/include/pca9685_params.h` and adopt them
if necessary.

If the active LOW output enable pin /OE is used, the GPIO has to be defined
as parameter, e.g.
```
     CFLAGS="-DPCA9685_PARAM_INT_PIN=\(GPIO\(0,6\)\)" \
     make -C tests/driver_pca9685 BOARD=...
```
