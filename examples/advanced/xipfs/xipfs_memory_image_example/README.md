# XiPFS mount point offboard memory image example

## About
This example demonstrates how to:
- create an XiPFS mountpoint memory image from a workstation,
- embed this latter into flash memory at compilation time,
- mount the memory image at runtime on board,
- execute files from this mountpoint within `shell`.

XiPFS should be compatible with ARM 32 bits platforms having an ARMv7-M Memory
Protection Unit; but for now, only the Qorvo `DWM1001` board has been tested
and is confirmed to function correctly.

## FAE files
[FAE format](https://github.com/2xs/fae_format/tree/master) is a custom
lightweight binary executable format dedicated to the deployment of
post-issuance software in constrained devices.

Two FAE files have been crafted and are provided for demonstration purposes:

- `pi.fae`, which computes and prints the first digits of Pi.
  Source code is available in the folder
  [03-pi](https://github.com/2xs/riot-xipfs-demonstrations/tree/master/03-pi)
  of the XiPFS demonstrations.
- `led.fae`, which switches on and off board leds.
  Source code is available in the folder
  [04-led](https://github.com/2xs/riot-xipfs-demonstrations/tree/master/04-led)
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

Given the following tree structure, crafted beforehand, and residing into the
host filesystem:

```shell
tree /tmp/test/
/tmp/test/
├── empty_dir
├── executables
│   ├── led.fae
│   └── pi.fae
└── regular_file.txt

3 directories, 3 files
```

Let's build a 20 KiB memory image from this directory, for the `dwm1001` board:

```shell
xipfs/tools/bin/mkxipfs --target dwm1001 build /path_to/RIOT/examples/advanced/xipfs/xipfs_memory_image_example/nvme0p0.flash /tmp/test 20K
```

## RIOT example usage

The target board is Qorvo's `dwm1001`:

- Open e.g. `pyterm` in a terminal with `make term`.

```shell
cd examples/advanced/xipfs/xipfs_memory_image_example
BOARD=dwm1001 QUIET=0 make term
```

- Build the example and flash the board in **another terminal**.

```shell
cd examples/advanced/xipfs/xipfs_memory_image_example
BOARD=dwm1001 QUIET=0 make flash
```

- Observe the welcome message and mountpoint log in `pyterm` terminal.

```
# main(): This is RIOT!
# vfs_mount: "/nvme0p0": OK
```

- Inspect the provided mountpoint.

```
> vfs df
# vfs df
# Mountpoint              Total         Used    Available     Use%
# /nvme0p0               20 KiB       16 KiB        4 KiB      80%

> vfs ls /nvme0p0
# vfs ls /nvme0p0
# empty_dir//
# executables//
# regular_file.txt      7 B
# total 1 files

> vfs ls /nvme0p0/empty_dir/
# vfs ls /nvme0p0/empty_dir/
#
# total 0 files

> vfs ls /nvme0p0/executables
# vfs ls /nvme0p0/executables
# pi.fae        1280 B
# led.fae       1824 B
# total 2 files
```

- Execute files with and without memory protection.

```
> execute /nvme0p0/executables/pi.fae
# execute /nvme0p0/executables/pi.fae
# 314159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535

> execute -p /nvme0p0/executables/pi.fae
# execute -p /nvme0p0/executables/pi.fae
# 314159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535

> execute -p /nvme0p0/executables/led.fae green on
# execute -p /nvme0p0/executables/led.fae green on

> execute /nvme0p0/executables/led.fae green off
# execute /nvme0p0/executables/led.fae green off
```

Here, both FAE files behave, and do not attempt to access memory out of their
related areas; thus the MPU does not raise any memory violation.

- At last, you can run VFS shell commands.
For example:

```
> vfs r /nvme0p0/regular_file.txt
# vfs r /nvme0p0/regular_file.txt
# 00000000: 666f 6f62 6172 0a                        foobar.
# -- EOF --

> vfs w /nvme0p0/regular_file.txt ascii a baz
# vfs w /nvme0p0/regular_file.txt ascii a baz

> vfs r /nvme0p0/regular_file.txt
# vfs r /nvme0p0/regular_file.txt
# 00000000: 666f 6f62 6172 0a62 617a                 foobar.baz
# -- EOF --

> vfs rm /nvme0p0/regular_file.txt
# vfs rm /nvme0p0/regular_file.txt
# rm: unlink: /nvme0p0/regular_file.txt

> vfs df
# vfs df
# Mountpoint              Total         Used    Available     Use%
# /nvme0p0               20 KiB       12 KiB        8 KiB      60%
```
