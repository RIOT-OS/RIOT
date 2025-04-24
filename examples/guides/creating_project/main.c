/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * For many printing related things, such as the puts function here
 * we import stdio, depending on your board, platform or form of output
 * it then includes the right definitions without the need to
 * worry about the specific details.
 */
#include <stdio.h>

/*
 * This is the main function of the program.
 * It serves as the entry point for the program and gets called once your CPU is
 * initialized.
 *
 * The function returns an integer value, which is the exit status
 * of the program. A return value of 0 indicates that the program has finished
 * successfully.
 */
int main(void) {
  puts("Hello World!");

  return 0;
}
