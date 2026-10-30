# About

This test compares the output of the Epson ESC/POS driver with the output of
[python-escpos](https://github.com/python-escpos/python-escpos), from which the
driver was ported. It uses the dummy transport, so no printer is required.

# Usage

```
make BOARD=native all test
```

# Reference data

`vectors.h` is generated with `gen_vectors.py` from the python-escpos `Dummy`
printer (python-escpos requires Python >= 3.11):

```
./gen_vectors.py <path to python-escpos/src> > vectors.h
```

Intended deviations of the C driver from python-escpos are documented and
fixed up in `gen_vectors.py`.
