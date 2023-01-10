USB to serial adapter tools
================================

Tools for finding connected USB to serial adapter devices.

`ttys.py`
---------

Lists currently connected USB to serial adapters by searching through UDEV
that match the given filters formatted with in the specified format.

By default, all USB serial adapters present are printed as a markdown table.

### Usage

    usage: ttys.py [-h] [--most-recent] [--format FORMAT] [--serial SERIAL] [--driver DRIVER] [--model MODEL]
                   [--model-db MODEL_DB] [--vendor VENDOR] [--vendor-db VENDOR_DB] [--iface-num IFACE_NUM]
                   [--exclude-serial [EXCLUDE_SERIAL ...]]

### Output Formats

With the parameter `--format FORMAT` a different format than the default
markdown table can be selected, e.g. `json` results in JSON output and `path`
will print the paths of the matching TTYs without any formatting (useful for
scripting). The full list of formats can be obtained by running the script with
the `--help` parameter.

Note: Formats other than `json` and `table` can be combined. A script that
required both path and serial of TTYs could use:

```
./ttys.py --format path serial
```

This will output one TTY per line with the selected fields separated by space.
To use a different separator than space (e.g. to create CSV files), the option
`--format-sep` can be used. If a field value contains the separator, it will
be quoted and quotation chars inside will be escaped.

### Filtering

For each column in the table, there is a matching filtering option. Except for
the `--serial` filter, all filters expect regular expressions that are tried
to match against the value in the given column. The `--serial` filter however
only matches if the serial of the TTY matches the given value literally. A
TTY is considered matching if and only if all filters apply.

There is an additional `--exclude-serial` option that can be used to exclude
serial devices (even before any filters are checked). If this option is absent
and the environment variable `EXCLUDE_TTY_SERIAL` is set, the serials in the
variable (separated by space) are used instead. This is useful if some bogus
serial devices are present, such as configuration interfaces of fancy monitors.

### Limiting Results

The parameter `--most-recent` will only print the most recently connect serial.
It can be combined with filters, in which case it will print the most recently
connected TTY among those matching all filters.

### Exit Code

The script exits with 0 if at least one TTY was found and 1 otherwise.

### Chaining

By relying on the exit code, it is simple to test for multiple variants of the
same board that e.g. differ in the UART to USB interface in the shell, e.g.
using

    ./ttys.py <FILTER_VARIANT_1> || ./ttys.py <FILTER_VARIANT_2>

### Build System Integration

By adding

    TTY_BOARD_FILTER := <FILTER PARAMETERS>

to the `Makefile.include` of your board, running `make MOST_RECENT_PORT=1 term`
will connect to the most recently connected board matching the provided
filters. Refer to https://api.riot-os.org/flashing.html#multiple-boards-simple
for more details.

`list-ttys.sh`
--------------

A wrapper that runs `./ttys.py --format path` for backward compatibility.

### Usage

    ./list-ttys.sh


`find-tty.sh`
-------------

Prints the paths to all TTY interfaces whose serial matches any of the given
arguments literally.

It basically runs `ttys.py --format path --serial "$arg"` for every argument
`$arg` and is intended for backward compatibility, as directly interacting with
`ttys.py` is the more flexible approach.

### Usage

    ./find-tty.sh [serial_1] [serial_2] ... [serial_n]


Limitations
-----------

Only tested on Linux, and probably only works on Linux.
