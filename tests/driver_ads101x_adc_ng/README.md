# Test for TI ADS101x/ADS111x as ADC extension

## Overview

This application can be used for testing the ADS101x/ADS111x device driver
together with the Common ADC API. It uses the default parameter definition
from the file `$(RIOTBASE)/drivers/ads101x/include/ads101x_params.h`.

## Usage

The shell commands `init`, `single`, `burst`, ... allow the initialization and
reading of ADS101x/ADS111x channels with a certain full scale range.

The ADS101x/ADS111x variant as well as the I2C bus and the address
are defined by the standard parameter definition from the file
`$(RIOTBASE)/drivers/ads101x/include/ads101x_params.h`. The parameter
`ADS101X_PARAM_MUX_GAIN` is not used for the test application.

By default, an ADS1115 is used as test device. To use another ADS101x/ADS111x
variant, the parameter `DADS101X_PARAM_DEVICE` has to be overriden:
```
CFLAGS='-DADS101X_PARAM_DEVICE=ADS101X_DEV_ADS1013' \
make -C tests/driver_ads101x_adc_ng BOARD=...
```

Parameters like `ADS101X_PARAM_I2C` and `ADS101X_PARAM_ADDR` can also be
overridden from the make command line:
```
CFLAGS='-ADS101X_PARAM_I2C=I2C_DEV\(1\) -DADS101X_PARAM_ADDR=\(0x49\)'
make -C tests/driver_ads101x_adc_ng BOARD=...
```

**Please note:** ADS1115 can also be used to test all other device variants.
