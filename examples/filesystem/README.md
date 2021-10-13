# File system usage example

## Description

This basic example shows how to use a file system with RIOT in your embedded
application.

In particular, this example shows:

- how to mount/format/unmount a file system, either with spiffs, littlefs or
  constfs
- how to open/read/write/close a file with and without newlib

In RIOT, most file systems use a `mtd` as flash interface. So to use this
example one must define `MTD_0`. `MTD_0` is a pointer to a `mtd_dev_t`
instance.

This example uses `littlefs` as default file system on the whole `mtd`.
A `constfs` file system is also demonstrated with two files.

All the RIOT file systems are used through the `vfs` interface, and on most
platforms files can be accessed transparently with `open/close/read/write/...`
functions.
With newlib, `fopen/fclose/fread/fwrite/...` can also be used transparently.

## Shell commands

The following commands are available:

- `format`: should be called the first time only, it will format the `mtd`
  with the configured file system
- `mount`: mount the file system on the configured mount point (default is
  `/sda` but it can be configured with `FLASH_MOUNT_POINT` define). The
  `constfs` file system is mounted automatically on `/const` when the
   application starts
- `umount`: unmount `/sda`
- `cat <file>`: similarly to unix `cat` unix command, it prints the given
  `<file>` on stdout
- `tee <file> <str>`: similarly to `tee` unix command, it writes `<str>` in
  `<file>`

Besides of these commands, the default `vfs` commands can be used, for
instance:

- `vfs df`: shows all mountpoints and used/available file system space
- `vfs ls <path>`: list files

## Example on `native` with `constfs`

- Build and run the `filesystem` example application on the `native` target:

```
make -C examples/filesystem all term
[...]
main(): This is RIOT! (Version: 2018.04-devel-/examples/filesystem)
constfs mounted successfully
```

- List the available files in the `constfs` predefined partition:

```
> ls /const
ls /const
/hello-world
/hello-riot
total 2 files
```

- Print the content of the files:

```
> cat /const/hello-world
cat /const/hello-world
Hello World!
> cat /const/hello-riot
cat /const/hello-riot
Hello RIOT!
```

## Example on `native` with `fatfs`

- Unpack the provided image proviced in the `pkg_fatfs` test:

```
$ tar vxjf ../../tests/pkg_fatfs/riot_fatfs_disk.tar.bz2
riot_fatfs_disk.img
```

- Configure the application to use the file and the right geometry by adding
  these to the Makefile:

```
CFLAGS += -DMTD_NATIVE_FILENAME=\"riot_fatfs_disk.img\"
CFLAGS += -DMTD_PAGE_SIZE=512
CFLAGS += -DMTD_SECTOR_SIZE=512
CFLAGS += -DMTD_SECTOR_NUM=262144
```

- In the Makefile, comment the `littlefs2` USEMODULE line, and enable the
  `fatfs_vfs` line instead.

- Build and run the `filesystem` example application on the `native` target as above.

- Mount the external file system:

```
> mount
mount
/sda successfully mounted
```

- List the available files in the FAT partition:

```
> ls /sda
ls /sda
TEST.TXT
total 1 files
```

- You can also use the writing commands now to create and modify files; run
  `vfs` for instructions.
