# Bus Statistics Test Application

Frankly, this is more an application showcasing the bus stats module rather
than testing it. But given that it is only useful to developers when optimizing
drivers, it makes little sense to place it prominently in the examples
folder.

This app will enable all default SAUL sensors and actuators, as those often
use I2C and occasionally use SPI. However, to actually collect statistics,
stats collection has to be enabled for each I2C/SPI sensor/actuator by addition
of an XFA entry. This is done for BME680 and BMx280, if such a sensor is used.

The application will launch a shell with the `saul` and the `bus_stats` command
being available. Run `bus_stats` command without arguments to get a listing of
all bus statistics for all monitored endpoints (both SPI and I2C); this list
will be empty unless a BMx280 or a BME680 is used, unless the required XFA
entries have been added.

The intention is to cause some I2C/SPI traffic by reading the monitored sensors
or controlling the monitored actuators via the `saul` command and then to
observe the changes in the bus stats.
