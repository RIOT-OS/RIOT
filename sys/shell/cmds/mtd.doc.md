@defgroup   sys_shell_commands_mtd MTD shell command
@addtogroup sys_shell_commands
@brief      MTD shell command for low level storage device access

Available mtd sub commands
==========================

There are different sub commands available to read and write data to and from an MTD.
Additional subcommands for example print information or control power states.

Subcommand: read <addr> <len>
-----------------------------

The subcommand consumes a memory address and a length in bytes of how many bytes to read.
@see mtd_read

Subcommand: read_page <page> <offset> <len>
-------------------------------------------

The subcommand consumes a page number, an offset withing that page and a length in bytes
of how many bytes to read.
@see mtd_read_page

Subcommand: write [-b] <addr> <data>
------------------------------------

The subcommand consumes a memory address and data to write.
If the optional parameter `-b` is used, it is expected that the provided data only
consists of hexadecimal digits, else it is expected that the data is a string.
@see mtd_write

Subcommand: write_page_raw [-b] <page> <offset> <data>
------------------------------------------------------

The subcommand consumes a page number, a byte offset within the page and data to write.
If the optional parameter `-b` is used, it is expected that the provided data only
consists of hexadecimal digits, else it is expected that the data is a string.
@see mtd_write_page_raw

Subcommand: write_page [-b] <page> <offset> <data>
--------------------------------------------------

The subcommand consumes a page number, a byte offset within the page and data to write.
If the optional parameter `-b` is used, it is expected that the provided data only
consists of hexadecimal digits, else it is expected that the data is a string.
The command only can be used when the module `mtd_write_page` is used.
@see mtd_write_page

Subcommand: erase <addr> <len>
------------------------------

The subcommand consumes a memory address and a length in bytes of how many bytes to erase.
@see mtd_erase

Subcommand: erase <sector> [count]
----------------------------------

The subcommand consumes a sector number and additionally a number of bytes to erase from this sector.
@see mtd_erase_sector

Subcommand: info
----------------

The subcommand consumes no additional arguments.
Information about storage size is printed.


Subcommand: power <on|off>
--------------------------

The subcommand consumes consumes one parameter to power the MTD on or off, if it is implemented by the MTD driver.
@see mtd_power
