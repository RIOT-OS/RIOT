# File system usage example

## Description

This basic example shows how to use a file system with RIOT in your embedded
application.

In particular, this example shows:

- how to mount/format/unmount a file system, either with constfs for all
  devices and the default fs for devices that have a filesystem mountpoint
  configured by the board.
- how to open/read/write/close a file with and without newlib

A `constfs` file system is demonstrated with two files for all targets
to demonstrate the file system functionality without an external mass
storage device.

For targets that feature an onboard external mass storage device with a
mount point configured, like the Nordic nRF52840 Development Kit
(see `boards/nrf52840dk`), the flash chip is automatically added by VFS.


All the RIOT file systems are used through the `vfs` interface, and on most
platforms files can be accessed transparently with `open/close/read/write/...`
functions.
With newlib, `fopen/fclose/fread/fwrite/...` can also be used transparently.

## Shell commands

The following commands are available:

- `vfs format /nvm0`: should be called the first time only, it will format the
  `/nvm0` mountpoint with the configured file system
- `vfs mount /nvm0`: mount the file system on the configured mount point
   The `constfs` file system is mounted automatically on `/const` when the
   application starts
- `vfs umount /nvm0`: unmount `/nvm0`
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

## Example on `nrf52840dk` with `littlefs`

- Build and flash `filesystem` example application of the `nrf52840dk` target

```
make BOARD=nrf52840dk -C examples/filesystem all flash
[...]
```

 - When connecting to the board via the serial console, it should display the
   following output:
```
main(): This is RIOT! (Version: 2024.04-devel-254-gee0f6d)
constfs mounted successfully
>
```

- The flash chip is empty by default and has to be formatted, which can be
  done with the `vfs format /nvm0` command. After that it has to be mounted
  with `vfs mount /nvm0`. Once the flash is formatted, it will be
  automatically mounted during boot.
```
> vfs format /nvm0
> vfs mount /nvm0
> vfs df
Mountpoint              Total         Used    Available     Use%
/const                   27 B         27 B          0 B     100%
/nvm0                   8 MiB        8 KiB     8184 KiB       0%
```

- After formatting, the flash is empty, but new files can be created
  with `tee` and printed with `cat`.

```
> ls /nvm0
./
../
total 0 files
>
> tee /nvm0/testfile "This is written to the flash!"
> cat /nvm0/testfile
This is written to the flash!>
> vfs ls /nvm0
./
../
testfile	30 B
total 1 files
```
