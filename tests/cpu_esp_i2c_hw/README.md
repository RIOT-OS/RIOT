# ESP Hardware I2C Test

## About

The application corresponds exactly to the application `tests/periph_i2c`.
It is only for testing the timer implementation using hardware counters.
In fact, the application is nothing more than the `tests/periph_i2c`
application compiled with the following command:

```
USEMODULE=esp_i2c_hw make -C tests/periph_i2c
```

## Expected Results

Since the application corresponds exactly to the application `tests/periph_i2c`,
it can be used in same way.
