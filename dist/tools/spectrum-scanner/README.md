# RIOT Spectrum Scanner Application

## About

This script can be used to plot the radio signal spectrum when a connected node
is running [spectrum-scanner application](https://github.com/RIOT-OS/applications/tree/master/spectrum-scanner)
application located in [RIOTs application repository](https://github.com/RIOT-OS/applications).
This node scans over the available radio channels performing CCA measurements
and outputting the measured ED level via a serial port. This output is then
parsed by the `plot_spectrum.py` script included in this folder run on a host computer.

## Dependencies

The `plot_spectrum.py` script requires [pyserial](https://pypi.python.org/pypi/pyserial)
for the serial port access, and [matplotlib](https://matplotlib.org) and
[numpy](http://www.numpy.org) for the plotting functionality.

Installing the dependencies:

#### Debuntu
    apt-get install python-serial

#### PIP
    pip install pyserial matplotlib numpy

## Usage

General usage:

1. Flash a RIOT node with the spectrum-scanner application from
(https://github.com/RIOT-OS/applications/tree/master/spectrum-scanner)

2. Run the `plot_spectrum.py` script
```
$ ./plot_spectrum.py <tty> -b <baudrate>
```

### Examples

![Example screenshot](https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/spectrum-scanner/example.png)
