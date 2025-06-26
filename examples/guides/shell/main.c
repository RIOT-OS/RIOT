/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "shell.h"

/* Our command we want to register to the shell */
int echo_command(int argc, char **argv) {
  /* We take the arguments passed to the command */
  for (int i = 1; i < argc; i++) {
    /* We print each argument followed by a space */
    printf("%s ", argv[i]);
  }
  /* Finally, we print a newline character to end the line */
  printf("\n");

  /* Return 0 to indicate success */
  return 0;
}

/* This little macro registers the command so we can use it in the shell */
SHELL_COMMAND(echo, "Echo a message", echo_command);

int main(void) {
  /* Buffer to store command line input */
  char buffer[SHELL_DEFAULT_BUFSIZE];

  /* Start the shell */
  shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

  /* Return 0 to indicate that the program has finished successfully */
  return 0;
}
