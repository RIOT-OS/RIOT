# About
This is a manual test application for the ST LSM6DSL sensor driver.

# Usage

After successful initialization the following tests are done :
    * Acc power down
    * Gyro power down
    * wait 1 second
    * Acc power up
    * Gyro power up
    * Enable latched interrupt
    * Disable latched interrupt
    * Read Acc
    * Read Gyro
    * Read Temp

If all previous steps are OK, a shell is launched and the user can do several
things with the `lsm6dsl` command. For the following commands/examples you can
read the `usage` by entering the command needed without parameters.

Here are some examples:

 * Read acc value :
```
    lsm6dsl read acc
```
 * Configure wake up interrupt (adapt parameters values to your usage):
```
lsm6dsl configure  wkup_int 0 2 1 0
```
 * Enable wake up interrupt on pin 0:
```
lsm6dsl enable wkup_int 0
```
 * Disable wake up interrupt on pin 0:
```
lsm6dsl disable wkup_int 0
```
 * Get reason for wake up alert :
```
lsm6dsl get_reason wkup
```

Note: reading sensor values may fail even on successful initialization.
