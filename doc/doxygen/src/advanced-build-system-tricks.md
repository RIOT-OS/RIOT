Advanced build system tricks                    {#advanced-build-system-tricks}
============================

[TOC]

Introduction                                                    {#introduction}
============

This page describes some build systems tricks that can help developers but are
not part of the standard workflow.

They are low level commands that should not be taken as part of a stable API
but better have a documentation than only having a description in the build
system code.


Customize the build system                            {#customize-build-system}
==========================

+ `RIOT_MAKEFILES_GLOBAL_PRE`: files parsed before the body of
  `$RIOTBASE/Makefile.include`
+ `RIOT_MAKEFILES_GLOBAL_POST`: files parsed after the body of
  `$RIOTBASE/Makefile.include`

The variables are a list of files that will be included by
`$RIOTBASE/Makefile.include`.
They will be handled as relative to the application directory if the path is
relative.


Usage
-----

You can configure your own files that will be parsed by the build system main
`Makefile.include` file before or after its main body, examples usages can be:

* Globally overwrite a variable, like `TERMPROG`
* Specify a hard written `PORT` / `DEBUG_ADAPTER_ID` for some BOARD values
* Define your custom targets
* Override default targets


Handling multiple boards with udev-rules                {#multiple-boards-udev}
========================================

When developing and working with multiple boards the default `PORT` configuration
for a particular board might not apply anymore so `PORT` will need to be specified
whenever calling `make term/test`. This can also happen if multiple `DEBUGGERS/PROGRAMMERS`
are present so `DEBUG_ADAPTER_ID` will also need to be passed. Keeping track of
this will become annoying.

One way of handling this is to use `udev` rules to define `SYMLINKS` between the
boards serial port (`riot/tty-<board-name>`) and the actual serial port
(dev/ttyACM* or other). With this we can query the rest of the boards serial
`dev` information (`DEBUG_ADAPTER_ID`, `PORT`, etc.) to always flash and open a
terminal on the correct port.

Procedure:

- use `udevadm info /dev/ttyACM0` to query the udev database for information on
  device on port `/dev/ttyACM0`.

  or use `udevadm info --attribute-walk --name /dev/ttyACM0` for more detailed
  output when the first level of information isn't enough

- create a udev rule with information of the device and one parent to create a
  matching rule in `/etc/udev/rules.d/70-riotboards.rules`.

~~~~~~~~~~~~~~~~~~~
    # samr21-xpro
    SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", \
    ATTRS{idProduct}=="2111", ATTRS{manufacturer}=="Atmel Corp.", \
    ATTRS{serial}=="ATML2127031800004957", SYMLINK+="riot/tty-samr21-xpro"
~~~~~~~~~~~~~~~~~~~

- reload rules: `udevadm control --reload-rules`

- Boards `PORT` are symlinked to /dev/riot/tty-`board-name`.

- Create a `makefile.pre` that will query the real `PORT` and the `DEBUG_ADAPTER_ID`
  from the `SYMLINK` info

~~~~~~~~~~~~~~~~~~~
    PORT = /dev/riot/tty-$(BOARD)
    DEBUG_ADAPTER_ID = $(\
        shell udevadm info -q property $(PORT) |\
        sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
~~~~~~~~~~~~~~~~~~~

- You can now add `makefile.pre` to `RIOT_MAKEFILES_GLOBAL_PRE` as an environment
  variable or on each `make` call:

~~~~~~~~~~~~~~~~~~~
    $ RIOT_MAKEFILES_GLOBAL_PRE=/path/to/makefile.pre make -C examples/hello-world flash term
~~~~~~~~~~~~~~~~~~~

_note_: if set as an environment variable it would be a good idea to add a variable
to enable/disable it, e.g:

~~~~~~~~~~~~~~~~~~~
ifeq (1,$(ENABLE_LOCAL_BOARDS))
    PORT = /dev/riot/tty-$(BOARD)
    DEBUG_ADAPTER_ID = $(\
        shell udevadm info -q property $(PORT) |\
        sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
endif
~~~~~~~~~~~~~~~~~~~

Handling multiple versions of the same BOARD
-------------------------------------------

The above procedure works fine when handling different boards, but not
multiple times the same board, e.g: multiple `samr21-xpro`.

An option for this would be to add an identifier of that board to the mapped
`riot/tty-*`, there are multiple ways of handling this but in the end it means
having a way to identify every copy.

Another way would be to map the `DEBUG_ADAPTER_ID` in the name:

~~~~~~~~~~~~~~~~~~~
SYMLINK+="riot/node-$attr{serial}
~~~~~~~~~~~~~~~~~~~

But it will require to know in advance the serial number of each board you want
to use. Another option would be to add some kind of numbering and defining
multiple symlinks for each board. e.g. for `samr21-xpro` number `n`:

~~~~~~~~~~~~~~~~~~~
    # samr21-xpro
    SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", \
    ATTRS{idProduct}=="2111", ATTRS{manufacturer}=="Atmel Corp.", \
    ATTRS{serial}=="ATML2127031800004957", SYMLINK+="riot/tty-samr21-xpro", \
    SYMLINK+="riot/tty-samr21-xpro-n"
~~~~~~~~~~~~~~~~~~~

Then, when flashing, the number can be specified and the parsing adapted:

~~~~~~~~~~~~~~~~~~~
    ifneq(,$(BOARD_NUM))
      PORT = /dev/riot/tty-$(BOARD)-$(BOARD_NUM)
    else
      PORT = /dev/riot/tty-$(BOARD)
    endif
    DEBUG_ADAPTER_ID = $(\
      shell udevadm info -q property $(PORT) |\
      sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~
    BOARD=samr21-xpro BOARD_NUM=n make flash term
~~~~~~~~~~~~~~~~~~~

In the end, this would be the same as using the serial, but a simple number might
be easier to handle.

Notes
-----
Udev only parses SUBSYSTEM and one parent. For others, we will rely on ENV
variables defined by 60-serial.rules

So the current filename should be higher than 60-serial.rules

If for some reason re-writing the serial is needed there is a windows tool:
  https://remoteqth.com/wiki/index.php?page=How+to+set+usb+device+SerialNumber

Documentation:
--------------
* The whole documentation
  http://reactivated.net/writing_udev_rules.html#udevinfo
* Udev manpage
  http://manpages.ubuntu.com/manpages/eoan/en/man7/udev.7.html


Analyze dependency resolution                   {#analyze-depedency-resolution}
=============================

When refactoring dependency handling or modifying variables used for dependency
resolution, one may want to evaluate the impact on the existing applications.
This describe some debug targets to dump variables used during dependency
resolution.

To analyze one board and application run the following commands in an
application directory.

Generate the variables dump with the normal dependency resolution to a
`dependencies_info_board_name` file:

~~~~~~~~~~~~~~~~~~~
BOARD=board_name make dependency-debug
~~~~~~~~~~~~~~~~~~~

Or with the "quick" version used by murdock to know supported boards
(this is an incomplete resolution, details in `makefiles/dependencies_debug.inc.mk`)
to a `dependencies_info-boards-supported_board_name` file:

~~~~~~~~~~~~~~~~~~~
BOARDS=board_name DEPENDENCY_DEBUG=1 make info-boards-supported
~~~~~~~~~~~~~~~~~~~

For more configuration and usage details, see in the file defining the targets
`makefiles/dependencies_debug.inc.mk`

To do a repository wide analysis, you can use the script
`dist/tools/buildsystem_sanity_check/save_all_dependencies_resolution_variables.sh`
that will generate the output for all boards and applications.
It currently take around 2 hours on an 8 cores machine with ssd.
