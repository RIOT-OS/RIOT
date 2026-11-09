# About

This test compares the output of the Epson ESC/POS driver with the output of
[python-escpos](https://github.com/python-escpos/python-escpos), from which the
driver was ported. It uses a dummy transport writing into a memory buffer, so
no printer is required.

# Usage

```
make BOARD=native all test
```

# Reference data

`vectors.h` is generated with `gen_vectors.py` from the python-escpos `Dummy`
printer. python-escpos requires Python >= 3.11 and has to be installed with
its `capabilities-data` submodule, e.g. into a virtual environment:

```
cd RIOT/tests/drivers/epson_escpos_dummy
git clone --recurse-submodules https://github.com/python-escpos/python-escpos
python3.11 -m venv venv
venv/bin/pip install ./python-escpos
venv/bin/python gen_vectors.py
```

The script writes `vectors.h` and records the python-escpos version in it. The
current vectors were generated with python-escpos commit `106e251` (version
`3.2.dev102+g106e251db`).

Intended deviations of the C driver from python-escpos are documented and
fixed up in `gen_vectors.py`.
