## About

This is a test application for the [SHA5461AS](https://www.datasheetcafe.com/wp-content/uploads/2019/01/SH5461AS-datasheet.pdf),
a 4 digits with 7 segments display.

## Connecting the device

The display needs to be connected as follows:

- Connect all the pins `X` to the board `SH5461AS_PARAM_X_PIN`
- Connect `DP` to the board `SHA5461AS_PARAM_DP_PIN`
- Connect all the pins `DIGX` to the board `SHA5461AS_PARAM_DIGX_PIN`

For each digit pin (`DIGX`), you need to use a transistor and link it to `GND`.

![datasheet image](https://www.datasheetcafe.com/wp-content/uploads/2015/09/SH5461AS.gif)

