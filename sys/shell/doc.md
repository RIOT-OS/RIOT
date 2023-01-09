/*
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_shell_commands Shell commands
 * @ingroup     sys_shell

Shell Commands
==============

Certain modules such as `ps`, `saul_reg`, or `gnrc_icmpv6_echo` can expose
(some of) their functionality not only as C-API, but also as shell command.
Using the module `shell_commands` will enable the shell integration of the used
modules, if it exists.

A few rarely needed shell commands that needs to be used in addition to the
`shell_commands` and the module providing the C-API. Examples include `nice`,
`gnrc_udp_cmd`, or `random_cmd`. Consult the documentation of the modules to
find out whether they have a shell integration and how to enable it.
 */
