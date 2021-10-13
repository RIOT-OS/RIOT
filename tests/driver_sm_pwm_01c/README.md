# Test Application for the Amphenol SM_PWM_01C infrared dust sensor

## About

This is a simple test application for the SM_PWM_01C driver.

## Expected result

If everything works then you should see the progress bar vary if dust
concentration changes in the room, or manually block the infrared inputs
to force a change.

Overly exaggerated values (blocked infrared sensor) can be seen here:

```
# main(): This is RIOT! (Version: 2021.04-devel-1044-gfd36c-HEAD)
# sm_pwm_01c driver test application
# starting weighted average PM2.5 and PM10 measurements
#
# PM2.5 level: 1272 ug/m3|███████████              |
# PM10  level:  490 ug/m3|████                     |
````

## Details

By default a moving average over a 30s window is returned ad the
read value. The progress bar is updated every 200ms.
