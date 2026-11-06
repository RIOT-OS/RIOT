/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef GUI_H
#define GUI_H

extern void create_window(int *argc, char ***argv);
extern void run_gui(void);
extern void done_program(int still_in_bootloader);

#endif /* GUI_H */
