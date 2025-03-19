# The eXecute In-Place File System

## Description

`xipfs` is a file system designed to streamline post-issuance software
deployment. `xipfs` allows direct execution of programs from flash
memory, eliminating the need for prior copying to RAM. This approach
conserves memory space and accelerates boot times, as the
microcontroller can run code directly from storage memory without
preloading into RAM.

The `xipfs` structure is based on a linked list, where each file
occupies at least one flash memory page. To prevent fragmentation, when
a file is deleted, subsequent files are shifted to fill the vacant
space.

`xipfs` is compatible with all microcontrollers featuring addressable
flash memory and most operating systems, provided they implement the
necessary functions to interact with the flash controller.

## Limitations

`xipfs` has the following limitations:

- No journaling: `xipfs` doesn't provide journaling. Without journaling,
  the file system cannot keep track of changes in a way that allows for
  recovery in the event of a crash or power failure. This can lead to
  data corruption and loss.

- No checksums: `xipfs` doesn't provide checksums. The lack of checksums
  means that the file system cannot verify the integrity of files. This
  increases the risk of undetected data corruption, as there is no
  mechanism to ensure that files have not been altered or damaged.

- Global file system lock: `xipfs` needs a global file system lock. A
  global file system lock can lead to performance bottlenecks, as it
  prevents multiple threads from accessing the file system
  simultaneously.

- Fixed file size: `xipfs` provide fixed file size. By default, a file
  created using `vfs_open(2)` has a fixed space reserved in flash that
  is the size of a flash page. This size cannot be extended later. To
  create a file larger than the fixed size of one flash page, the
  `mk(1)` command or the `xipfs_new_file(3)` function must be used.

- Limited character set: `xipfs` supports only a subset of 7-bit ASCII
  characters, specifically `[0-9A-Za-z\/\.\-_]`.

- Limited path length: `xipfs` maximum path length is 64 characters.

## Tested cards

`xipfs` is expected to be compatible with all boards that feature
addressable NVM. However, only the `DWM1001` board has been tested and
is confirmed to function correctly.

## Funding

The `xipfs` project is part of the TinyPART project funded by the
MESRI-BMBF German-French cybersecurity program under grant agreements
n°ANR-20-CYAL-0005 and 16KIS1395K.
