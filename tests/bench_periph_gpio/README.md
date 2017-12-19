## About
This application implements a benchmark for measuring the run-time of GPIO
driver functions. It is meant to be used when optimizing specific GPIO driver
implementations.

The output will look similar to this:
```
2017-08-16 13:07:18,728 - INFO # main(): This is RIOT! (Version: xxx)
2017-08-16 13:07:18,728 - INFO #
2017-08-16 13:07:18,733 - INFO # GPIO driver run-time performance benchmark
2017-08-16 13:07:18,733 - INFO #
2017-08-16 13:07:20,112 - INFO #    gpio_set:   1375004us  ---   1.375us per call
2017-08-16 13:07:21,493 - INFO #  gpio_clear:   1375004us  ---   1.375us per call
2017-08-16 13:07:23,435 - INFO # gpio_toggle:   1937504us  ---   1.937us per call
2017-08-16 13:07:25,127 - INFO #  gpio_write:   1687503us  ---   1.687us per call
2017-08-16 13:07:27,196 - INFO #   gpio_read:   2062504us  ---   2.062us per call
2017-08-16 13:07:27,196 - INFO #
2017-08-16 13:07:27,197 - INFO #  --- DONE ---
```
