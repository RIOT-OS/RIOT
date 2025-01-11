Black Magic Probe Helper script
================================

This script can detect [Black Magic Probes](https://github.com/blacksphere/blackmagic/wiki) and act as a flashloader (and more).
It is compatible with Linux and macOS.

All important options that can be set via the monitor command are available as arguments.

For example, SWD is the default protocol, but JTAG can be used by specifying `--jtag`.

```
usage: bmp.py [-h] [--jtag] [--swd] [--connect-srst] [--tpwr]
              [--serial SERIAL] [--port PORT] [--attach ATTACH]
              [--gdb-path GDB_PATH] [--term-cmd TERM_CMD]
              [{list,flash,erase,debug,term,reset}] [file]

Black Magic Tool helper script.

positional arguments:
  {list,flash,erase,debug,term,reset}
                        choose a task to perform
  file                  file to load to target (hex or elf)

options:
  -h, --help            show this help message and exit
  --jtag                use JTAG transport
  --swd                 use SWD transport (default)
  --connect-srst        reset target while connecting
  --tpwr                enable target power
  --serial SERIAL       choose specific probe by serial number
  --port PORT           choose specific probe by port (overrides auto
                        selection)
  --attach ATTACH       choose specific target by number
  --gdb-path GDB_PATH   path to GDB
  --bmp-version BMP_VERSION
                        choose specific firmware version
  --term-cmd TERM_CMD   serial terminal command
```

## Available actions
* `list` lists connected targets (default action)
* `flash` load file to target
* `erase` erase target flash
* `debug` start GDB shell that is attached to target
* `term` start TTY emulator program to look into connected UART
* `reset` reset target (using RST line)

## Probe selection
The probe is auto discovered based on the USB VID (0x1D50) and PID (0x6018,
0x6017). The first discovered probe will be selected, unless `--port` or
`--serial` is provided.

If `--port` is provided, then that port will be used as the GDB port for all
actions, except for the `term` action. `--port` also accepts values such as
network addresses.

## Supported firmwares
There are minor differences in the available firmwares of the Black Magic
debugger. Compatibility has been tested with version 1.8+.

This tool will try to determine which version of the firmware is installed,
unless the probe is accessed remotely (e.g. `--port` is a network address). If
the firmware version cannot be determined, it will assume version 1.10.2. This
can be overridden using the `--bmp-version` flag.

## Examples (tested with BluePill STM32F103F8C6)
* test connection:
```
user@pc:~$ ./bmp.py --connect-srst
found following Black Magic GDB servers:
	[/dev/ttyACM0] Serial: BDD391D6 <- default
connecting to [/dev/ttyACM0]...
connecting successful.
scanning using SWD...
found following targets:
	STM32F1 medium density M3/M4
```
* flashing:
```
user@pc:~$ ./bmp.py --connect-srst flash example.elf
found following Black Magic GDB servers:
	[/dev/ttyACM0] Serial: BDD391D6 <- default
connecting to [/dev/ttyACM0]...
connecting successful.
scanning using SWD...
found following targets:
	STM32F1 medium density M3/M4

attaching to target successful.
downloading... total size: 742.5K
downloading section [.text] (12.8K)                                            |
100%|##########################################################################|
downloading section [.relocate] (120B)                                         |
100%|##########################################################################|
downloading finished
checking flash successful.
killing successful.
```
* erasing:
```
user@pc:~$ ./bmp.py --connect-srst erase
found following Black Magic GDB servers:
	[/dev/ttyACM0] Serial: BDD391D6 <- default
connecting to [/dev/ttyACM0]...
connecting successful.
scanning using SWD...
found following targets:
	STM32F1 medium density M3/M4

attaching to target successful.
erasing...
erasing target successful.
```
* open GDB shell:
```
user@pc:~$ ./bmp.py --connect-srst debug example.elf
found following Black Magic GDB servers:
	[/dev/ttyACM0] Serial: BDD391D6 <- default
connecting to [/dev/ttyACM0]...
GNU gdb (Debian 8.2.1-2+b1) 8.2.1
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from /home/user/dev/helloriot/bin/bluepill/example.elf...done.
Remote debugging using /dev/ttyACM0
Assert SRST during connect: enabled
Target voltage: unknown
Available Targets:
No. Att Driver
 1      STM32F1 medium density M3/M4
Attaching to program: /home/user/dev/helloriot/bin/bluepill/example.elf, Remote target
reset_handler_default ()
    at /home/max/dev/RIOT/cpu/cortexm_common/vectors_cortexm.c:79
79	{
(gdb)
```
* open UART:
```
user@pc:~$ ./bmp.py --connect-srst term
main(): This is RIOT! (Version: 2019.10-devel-800-g4f5d6)
```
