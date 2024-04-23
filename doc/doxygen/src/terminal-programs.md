Terminal programs configuration                             {#terminal-programs}
===========================================================

[TOC]

Background                                                         {#background}
==========
This page explains how to configure some popular terminal programs for correct
display of newlines when using the serial interface of a RIOT powered device.
When printing something using *stdio* (e.g., `printf("Hello World!\n");`, RIOT
sends a line feed character (`0xA`) as `\n` (newline).

Some terminals need more, for example, a carriage return and a line feed
character (0xD, 0xA). See https://en.wikipedia.org/wiki/Newline for background.

This page tries to collect needed settings for common terminal programs that
will make them correctly display newlines.

gtkterm                                                               {#gtkterm}
======
- Graphical method:
    - Open the configuration menu.
    - Click on ***CR LF auto***.
- Manual method:
    - Edit the file `~/.gtktermrc`.
    - Change value of ***crlfauto*** option to `True`.

minicom                                                               {#minicom}
=======
- Interactive method:
    - Press ***Ctrl+A u***.
- Manual method:
    - Edit the configuration file (`~/.minirc.dfl` per default).
    - Add the following line:
    pu addcarreturn     Yes

miniterm                                                             {#miniterm}
========
- Generic method:
    - Start with `--eol CR`parameter.
- Via RIOT build system:
    - `RIOT_TERMINAL=miniterm make term`

picocom                                                               {#picocom}
=======
- Generic method:
    - Start with `--imap lfcrlf` parameter.
- Via RIOT build system:
    - `RIOT_TERMINAL=picocom make term`

putty                                                                   {#putty}
=====
- Graphical method:
    - Go to configuration tree and choose `Terminal` branch.
    - Enable option `Implicit CR in every LF`.
