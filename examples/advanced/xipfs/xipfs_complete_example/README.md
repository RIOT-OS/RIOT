# XiPFS complete example

## About
This example demonstrates:
- the allocation on an empty mountpoint in flash memory,
- the inclusion in flash memory of a pre-made offboard mountpoint image,
- how to mount these and use them in code.

XiPFS should be compatible with ARM 32 bits platforms having an ARMv7-M Memory Protection Unit;
but for now, only the Qorvo `DWM1001` board has been tested and is confirmed to function correctly.

## FAE files
[FAE format](https://github.com/2xs/fae_format/tree/master) is a custom
lightweight binary executable format dedicated to the deployment of
post-issuance software in constrained devices.

Two FAE files have been crafted and are provided for demonstration purposes:

- `21.fae`, which returns the integer value 21.
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

## `mkxipfs` tool

XiPFS offers to create mountpoint memory images offboard, thanks to a provided
tool running on workstations, namely `mkxipfs` binary.

### How to make `mkxipfs`
```shell
git clone https://github.com/2xs/xipfs.git
make -C xipfs/tools
ls xipfs/tools/bin
mkxipfs
```

More information and details can be found in the related XiPFS'
[README.md](https://github.com/2xs/xipfs/blob/main/README.md)
and [GETTING_STARTED.md](https://github.com/2xs/xipfs/blob/main/GETTING_STARTED.md).

### Building memory images easily

Given the following tree structure, crafted beforehand, and residing into the host filesystem:

```shell
tree /tmp/complete_example/
/tmp/complete_example/
├── 21.fae
└── dumper.fae

1 directory, 2 files
```

Let's build a 16 KiB memory image from this directory, for the `dwm1001` board:

```shell
xipfs/tools/bin/mkxipfs --target dwm1001 build /path_to/RIOT/examples/advanced/xipfs/xipfs_complete_example/nvme0p0.flash /tmp/complete_example 16K
```

## RIOT example usage

The target board is Qorvo's `dwm1001`:

- Open e.g. `pyterm` in a terminal with `make term`.

```shell
cd examples/advanced/xipfs/xipfs_complete_example
BOARD=dwm1001 QUIET=0 make term
```

- Build the example and flash the board in **another terminal**.

```shell
cd examples/advanced/xipfs/xipfs_complete_example
BOARD=dwm1001 QUIET=0 make flash
```

- Observe the messages displayed in `pyterm` terminal.

```
# main(): This is RIOT! (Version: ...)
# vfs_mount: "/nvme0p0": OK
# vfs_mount: "/nvme0p1": OK
#
# '/nvme0p1/bin' directory has been created.
# Executable '/nvme0p0/21.fae' file has been copied to '/nvme0p1/bin/twenty-one.fae'
# '/nvme0p0/21.fae' has been run and returned '21'.
# '/nvme0p1/bin/twenty-one.fae' has been run and returned '21'.
# The cumulated returned value of '/nvme0p0/21.fae' and '/nvme0p1/bin/twenty-one.fae' is equal to 42.
# '/nvme0p1/bin/twenty-one.fae' file has been erased.
# '/nvme0p1/bin' directory has been removed.
#
# XiPFS is now going to run '/nvme0p0/dumper.fae' with different arguments, guarded by the hardware MPU.
#
# Illegal attempt to read values from RAM in progress...
# 20000020  Illegal memory access detected at 0x20000020.
# Illegal attempt to read values from RAM has failed as expected.
#
# Legal attempt to read values from RAM in progress...
# 20003050  0a 00 00 00 88 66 00 20   00 30 00 20 00 00 00 00  |.....f. .0. ....|
# 20003060  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003070  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003080  00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00  |................|
# 20003090
# Legal attempt to read values from RAM has succeeded as expected.
#
# Illegal attempt to read values from ROM in progress...
# 00  Illegal memory access detected at 0x0.
# Illegal attempt to read values from ROM has failed as expected.
#
# Legal attempt to read values from ROM in progress...
# c484  0f b4 13 b5 04 a9 09 4b   51 f8 04 0b 01 91 5a f8  |.......KQ.....Z.|
# c494  03 40 23 68 9a 46 63 68   5b 68 98 47 a3 68 9a 46  |.@#h.Fch[h.G.h.F|
# c4a4  02 b0 bd e8 10 40 04 b0   70 47 00 bf 00 00 00 00  |.....@..pG......|
# c4b4  10 b5 05 4b 5a f8 03 40   23 68 9a 46 63 68 9b 68  |...KZ..@#h.Fch.h|
# c4c4
# Legal attempt to read values from ROM has succeeded as expected.
#
# Now, you can use vfs shell commands to manage '/nvme0p0' and '/nvme0p1' mount points.
```

- At last, you can then run regular VFS shell commands, as in other XiPFS examples.
```
> vfs df
# vfs df
# Mountpoint              Total         Used    Available     Use%
# /nvme0p0               16 KiB        8 KiB        8 KiB      50%
# /nvme0p1               40 KiB          0 B       40 KiB       0%

> vfs ls /nvme0p0
# vfs ls /nvme0p0
# 21.fae        1120 B
# dumper.fae    1760 B
# total 2 files

> vfs ls /nvme0p1
# vfs ls /nvme0p1
# total 0 files
```
