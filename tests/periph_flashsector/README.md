Expected result
===============
Use the provided shell commands, to read and write sectors from/to the MCU's
internal flash memory. For altering the data in a flash sector, use a sequence
similar to this:
- read some sector from the flash, this will load the first 1024 byte into a
local buffer
```
read 2 1024
```
- edit the contents of the local buffer, here we write 'Hello_RIOT' to position
  100
```
edit 100 Hello_RIOT
```
- write the local buffer to any target sector in the flash. CAUTION: if you
  override any sector, that contains program code (or even the interrupt vector),
  you will most like encounter hard faults and crashes which can only be fixed
  by re-flashing the node...
```
write 2
```
- check if the contents were written, dump the first 1024 byte
```
dump 2 1024
```
- now power off the node, wait a bit and power it back on. The contents of the
  sector written previously should still be there

What else to check:
- Erase a sector with previously known contents, to make sure the erasing works
- also check the sectors before and after the targeted sector, to see if the
  sector size is correct, and that you are only erasing the actual sector and
  not any parts of the neighboring sector.

Background
==========
This test provides you with tools to test implementations of the `flashsector`
peripheral driver interface.
