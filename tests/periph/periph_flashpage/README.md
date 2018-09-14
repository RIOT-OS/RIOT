Expected result
===============
Use the provided shell commands to read and write pages from/to the MCU's
internal flash memory. For altering the data in a flash page, use a sequence
similar to this:
- read some page from the flash, this will load this page into a local buffer
```
read 100
```
- edit the contents of the local buffer, here we write 'Hello_RIOT' to position
  100
```
edit 100 Hello_RIOT
```
- write the local buffer to any target page in the flash. CAUTION: if you
  override any page, that contains program code (or even the interrupt vector),
  you will most like encounter hard faults and crashes which can only be fixed
  by re-flashing the node...
```
write 100
```
- check if the contents were written
```
dump 100
```
- now power off the node, wait a bit and power it back on. The contents of the
  page written previously should still be there

What else to check:
- Erase a page with previously known contents, to make sure the erasing works
- also check the pages before and after the targeted page, to see if the page
  size is correct, and that you are only erasing the actual page and not any
  parts of the neighboring page.

Background
==========
This test application provides shell commands to verify the implementations of
the `flashpage` peripheral driver interface.
