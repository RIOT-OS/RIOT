driver_pms5003
==============

This application provides a basic CLI to test the PMS5003 driver. The
default BOARD for the test is the adafruit feather-nrf52840 since
there is expansion BOARD with an interface for the sensor [enviro-plus-featherwing](https://shop.pimoroni.com/products/enviro-plus-featherwing).

Usage
=====

The following commands are available:

```
help
Command              Description
---------------------------------------
read                 Read sensor data
print                Print sensor data
mode                 Set sensor mode <active|passive>
```

If in active mode sensor data is continuously read as the device outputs it.
In passive mode this is done upon requests. The `print` command allows to
check this since the without performing a read the current timestamp and
value for the measurement will change:

```
> mode active
mode active
> print
print
pms5003 PM data_ptr measured at: 30862603
        PM1.0: 3 ug/m3 (ultrafine particles)
        PM2.5: 5 ug/m3 (combustion particles, organic compounds, metals)
        PM10: 6 ug/m3 (dust, pollen, mould spores)
        Num particles <= 0.3µm: 738/cm^3
        Num particles <= 0.5µm: 202/cm^3
        Num particles <= 1µm: 34/cm^3
        Num particles <= 2p5µm: 10/cm^3
        Num particles <= 5µm: 2/cm^3
        Num particles <= 10µm: 0/cm^3
> print
print
pms5003 PM data_ptr measured at: 31710979
        PM1.0: 2 ug/m3 (ultrafine particles)
        PM2.5: 4 ug/m3 (combustion particles, organic compounds, metals)
        PM10: 5 ug/m3 (dust, pollen, mould spores)
        Num particles <= 0.3µm: 738/cm^3
        Num particles <= 0.5µm: 202/cm^3
        Num particles <= 1µm: 34/cm^3
        Num particles <= 2p5µm: 10/cm^3
        Num particles <= 5µm: 2/cm^3
        Num particles <= 10µm: 0/cm^3
```

If in passive mode you must issue a `read` command for the values to change

```
> mode passive
mode passive
> print
print
pms5003 PM data_ptr measured at: 64881358
        PM1.0: 2 ug/m3 (ultrafine particles)
        PM2.5: 3 ug/m3 (combustion particles, organic compounds, metals)
        PM10: 4 ug/m3 (dust, pollen, mould spores)
        Num particles <= 0.3µm: 636/cm^3
        Num particles <= 0.5µm: 182/cm^3
        Num particles <= 1µm: 24/cm^3
        Num particles <= 2p5µm: 2/cm^3
        Num particles <= 5µm: 2/cm^3
        Num particles <= 10µm: 0/cm^3
> print
print
pms5003 PM data_ptr measured at: 64881358
        PM1.0: 2 ug/m3 (ultrafine particles)
        PM2.5: 3 ug/m3 (combustion particles, organic compounds, metals)
        PM10: 4 ug/m3 (dust, pollen, mould spores)
        Num particles <= 0.3µm: 636/cm^3
        Num particles <= 0.5µm: 182/cm^3
        Num particles <= 1µm: 24/cm^3
        Num particles <= 2p5µm: 2/cm^3
        Num particles <= 5µm: 2/cm^3
        Num particles <= 10µm: 0/cm^3
> read
read
pms5003 PM data_ptr measured at: 71457134
        PM1.0: 2 ug/m3 (ultrafine particles)
        PM2.5: 3 ug/m3 (combustion particles, organic compounds, metals)
        PM10: 4 ug/m3 (dust, pollen, mould spores)
        Num particles <= 0.3µm: 648/cm^3
        Num particles <= 0.5µm: 190/cm^3
        Num particles <= 1µm: 24/cm^3
        Num particles <= 2p5µm: 2/cm^3
        Num particles <= 5µm: 2/cm^3
        Num particles <= 10µm: 0/cm^3
```
