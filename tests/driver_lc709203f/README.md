# About
This is a test application for the ON Semiconductor lc709203F Battery Fuel Gauge
# Usage
Just enter the `make BOARD=??? flash` command in the `tests/driver_lc709203F/` folder.
Make sure the `LC709203F_INT_PIN` is set in your boards periph_conf.h
# Results
The sensor will first test the low RSOC interrupt by setting the threshold limit to 100%. Therefore a low RSOC interrupt should be triggered and the message "ALARM low RSOC" should be printed to the terminal. After that all major measurements will be printed and refreshed every 5s.
