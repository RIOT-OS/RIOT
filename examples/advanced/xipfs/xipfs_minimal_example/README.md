# XiPFS minimal example

## About
This example demonstrates how to:
- declare 2 XiPFS mountpoints, `nvme0p0` and `nvme0p1`,
  both located in flash memory (`.flash_writable` section),
- mount them at runtime,
- populate one of them with embedded executable files,
- execute files with and without memory protection from `shell`.

XiPFS should be compatible with ARM 32 bits platforms having an ARMv7-M Memory
Protection Unit; but for now, only the Qorvo `DWM1001` board has been tested
and is confirmed to function correctly.

## FAE files
[FAE format](https://github.com/2xs/fae_format/tree/master) is a custom
lightweight binary executable format dedicated to the deployment of
post-issuance software in constrained devices.

Two FAE files have been crafted and are provided for demonstration purposes:

- `hello-world.fae`, which prints the traditional "Hello World!" message and
  displays the current temperature.
  Source code is available in the folder
  [01-main](https://github.com/2xs/riot-xipfs-demonstrations/tree/master/01-main)
  of the XiPFS demonstrations.
- `dumper.fae`, which attempts to display to standard output memory contents
  from RAM or flash memory.
  Source code is available in the folder
  [18-memory_hexdump](https://github.com/2xs/riot-xipfs-demonstrations/tree/master/18-memory_hexdump)
  of the XiPFS demonstrations.

To reproduce these files or to create your own, please start by cloning
the `master` branch of [FAE format repository](https://github.com/2xs/fae_format/tree/master).

Then, read the associated [README.md](https://github.com/2xs/fae_format/blob/master/README.md)
and [GETTING_STARTED.md](https://github.com/2xs/fae_format/blob/master/GETTING_STARTED.md)
documents to setup your build environment.

Finally, clone the `master` branch of
[XiPFS demonstrations repository](https://github.com/2xs/riot-xipfs-demonstrations/tree/master)
and follow the
[README.md file](https://github.com/2xs/riot-xipfs-demonstrations/blob/master/README.md).

## RIOT example usage
Given the target board being Qorvo's `dwm1001`:

- Open e.g. `pyterm` in a terminal with `make term`.

```shell
cd RIOT/examples/advanced/xipfs/xipfs_minimal_example
BOARD=dwm1001 QUIET=0 make term
```

- Build the example and flash the board in **another terminal**.

```shell
cd RIOT/examples/advanced/xipfs/xipfs_minimal_example
BOARD=dwm1001 QUIET=0 make flash
```

- Observe the welcome message and mountpoints log in `pyterm` terminal.

```
# main(): This is RIOT!
# vfs_mount: "/nvme0p0": OK
# vfs_mount: "/nvme0p1": OK
```

- Inspect the two empty mountpoints.

```
> vfs ls /nvme0p0
# vfs ls /nvme0p0
# total 0 files
> vfs ls /nvme0p1
# vfs ls /nvme0p1
# total 0 files
```

- Drop files into the first partition thanks to the special example
command `drop_files`.

```
> drop_files
# drop_files
> vfs ls /nvme0p0
# vfs ls /nvme0p0
# hello-world.fae       1280 B
# dumper.fae    1760 B
# total 2 files
```

- Execute `hello-world.fae` file without memory protection.

```
> execute /nvme0p0/hello-world.fae
# execute /nvme0p0/hello-world.fae
# Hello World!
# The temperature is currently 27.50°C.
```

- Execute `dumper.fae` without memory protection.

```
> execute /nvme0p0/dumper.fae legit-ram
# execute /nvme0p0/dumper.fae legit-ram
# 20003050  f0 24 00 20 a8 66 00 20   00 30 00 20 00 00 00 00  |.$. .f. .0. ....|
# 20003060  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003070  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003080  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003090

> execute /nvme0p0/dumper.fae non-legit-ram
# execute /nvme0p0/dumper.fae non-legit-ram
# 20000020  fe e7 fe e7 fe e7 fe e7   fe e7 fe e7 fe e7 fe e7  |................|
# 20000030  fe e7 fe e7 fe e7 fe e7   fe e7 fe e7 fe e7 fe e7  |................|
# 20000040  fe e7 fe e7 fe e7 fe e7   fe e7 fe e7 fe e7 fe e7  |................|
# 20000050  fe e7 fe e7 fe e7 fe e7   fe e7 fe e7 fe e7 fe e7  |................|
# 20000060

> execute /nvme0p0/dumper.fae legit-rom
# execute /nvme0p0/dumper.fae legit-rom
# 10484  0f b4 13 b5 04 a9 09 4b   51 f8 04 0b 01 91 5a f8  |.......KQ.....Z.|
# 10494  03 40 23 68 9a 46 63 68   5b 68 98 47 a3 68 9a 46  |.@#h.Fch[h.G.h.F|
# 104a4  02 b0 bd e8 10 40 04 b0   70 47 00 bf 00 00 00 00  |.....@..pG......|
# 104b4  10 b5 05 4b 5a f8 03 40   23 68 9a 46 63 68 9b 68  |...KZ..@#h.Fch.h|
# 104c4

> execute /nvme0p0/dumper.fae non-legit-rom
# execute /nvme0p0/dumper.fae non-legit-rom
# 00  00 04 00 20 11 0a 00 00   bd 09 00 00 79 09 00 00  |... ........y...|
# 10  75 0c 00 00 cd 09 00 00   dd 09 00 00 00 00 00 00  |u...............|
# 20  00 00 00 00 00 00 00 00   00 00 00 00 35 09 00 00  |............5...|
# 30  ed 09 00 00 00 00 00 00   e9 08 00 00 fd 09 00 00  |................|
# 40
```

Please note that `dumper.fae` is able to **access memory anywhere in
both RAM and Flash memory**.

- Execute `dumper.fae` with memory protection enabled.

```
> execute -p /nvme0p0/dumper.fae legit-ram
# execute -p /nvme0p0/dumper.fae legit-ram
# 20003050  e8 24 00 20 88 66 00 20   00 30 00 20 00 00 00 00  |.$. .f. .0. ....|
# 20003060  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003070  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003080  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003090

> execute -p /nvme0p0/dumper.fae non-legit-ram
# execute -p /nvme0p0/dumper.fae non-legit-ram
# 20000020  Illegal memory access detected at 0x20000020.
# Failed to execute '/nvme0p0/dumper.fae', error=-5 (I/O error)

> execute -p /nvme0p0/dumper.fae legit-rom
# execute -p /nvme0p0/dumper.fae legit-rom
# 10484  0f b4 13 b5 04 a9 09 4b   51 f8 04 0b 01 91 5a f8  |.......KQ.....Z.|
# 10494  03 40 23 68 9a 46 63 68   5b 68 98 47 a3 68 9a 46  |.@#h.Fch[h.G.h.F|
# 104a4  02 b0 bd e8 10 40 04 b0   70 47 00 bf 00 00 00 00  |.....@..pG......|
# 104b4  10 b5 05 4b 5a f8 03 40   23 68 9a 46 63 68 9b 68  |...KZ..@#h.Fch.h|
# 104c4

> execute -p /nvme0p0/dumper.fae non-legit-rom
# execute -p /nvme0p0/dumper.fae non-legit-rom
# 00  Illegal memory access detected at 0x0.
# Failed to execute '/nvme0p0/dumper.fae', error=-5 (I/O error)
```

Please note that `dumper.fae` is **only able to access to its own RAM and
Flash memory spaces**.

**Other attempts** lead to an **illegal memory access**, detected and raised
by the board MPU hardware.

- At last, you can run VFS shell commands on XiPFS mountpoints.
For example:

```
> vfs df
# vfs df
# Mountpoint              Total         Used    Available     Use%
# /nvme0p0               40 KiB        8 KiB       32 KiB      20%
# /nvme0p1               60 KiB          0 B       60 KiB       0%

> vfs w /nvme0p1/test_0.txt ascii a 1234
# vfs w /nvme0p1/test_0.txt ascii a 1234

> vfs w /nvme0p1/test_1.txt ascii o foobar
# vfs w /nvme0p1/test_1.txt ascii o foobar

> vfs ls /nvme0p1
# vfs ls /nvme0p1
# test_0.txt    4 B
# test_1.txt    6 B
# total 2 files

> vfs df /nvme0p1
# vfs df /nvme0p1
# Mountpoint              Total         Used    Available     Use%
# /nvme0p1               60 KiB        8 KiB       52 KiB      13%

> vfs mkdir /nvme0p1/my_dir
# vfs mkdir /nvme0p1/my_dir
# mkdir: mkdir: /nvme0p1/my_dir

> vfs mv /nvme0p1/test_0.txt /nvme0p1/my_dir/test_0_moved.txt
# vfs mv /nvme0p1/test_0.txt /nvme0p1/my_dir/test_0_moved.txt
# mv: move src: /nvme0p1/test_0.txt dest: /nvme0p1/my_dir/test_0_moved.txt

> vfs w /nvme0p1/my_dir/foobar.txt ascii o baz
# vfs w /nvme0p1/my_dir/foobar.txt ascii o baz

> vfs ls /nvme0p1
# vfs ls /nvme0p1
# my_dir//
# test_1.txt    6 B
# total 1 files

> vfs ls /nvme0p1/my_dir
# vfs ls /nvme0p1/my_dir
# test_0_moved.txt      4 B
# foobar.txt    3 B
# total 2 files

> vfs df
# vfs df
# Mountpoint              Total         Used    Available     Use%
# /nvme0p0               40 KiB        8 KiB       32 KiB      20%
# /nvme0p1               60 KiB       12 KiB       48 KiB      20%
```
