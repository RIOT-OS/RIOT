driver_hm330x
==============

This is a simple test application for the HM330X I2C particle sensor. The test
will read and print the sensor data every second.


Note that particle counting is only available with hm3302.

- `hm3302` output;

```
main(): This is RIOT! (Version: 2021.04-devel-1342-g93325-pr_driver_hm330x)
HM330X test application
+------------Initializing------------+
+------------------------+------------------------+----------------------------------------------+
| Standard concentration | Atmospheric Environment|   # Particles in 0.1l air of diameter >=     |
| PM1.0 | PM2.5 | PM10.0 | PM1.0 | PM2.5 | PM10.0 | 0.3µm | 0.5µm | 1.0µm | 2.5µm | 5.0µm | 10µm |
+-------+-------+--------+-------+-------+--------+-------+-------+-------+-------+-------+------+
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
|      5|      7|       7|      5|      7|       7|      0|      0|      0|      0|      0|     0|
```

- `hm3301` output;

```
main(): This is RIOT! (Version: )
HM330X test application
+------------Initializing------------+
+------------------------+------------------------+
| Standard concentration | Atmospheric Environment|
| PM1.0 | PM2.5 | PM10.0 | PM1.0 | PM2.5 | PM10.0 |
+-------+-------+--------+-------+-------+--------+
|      8|     11|      11|      8|     11|      11|
|      8|     11|      11|      8|     11|      11|
|      8|     11|      11|      8|     11|      11|
|      9|     13|      13|      9|     13|      13|
|      9|     13|      13|      9|     13|      13|
|      9|     13|      13|      9|     13|      13|
|      9|     13|      13|      9|     13|      13|
|      9|     13|      13|      9|     13|      13|
```
