
/*
 * The olsr.org Optimized Link-State Routing daemon(olsrd)
 * Copyright (c) 2004-2011, the olsr.org team - see HISTORY file
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include "cunit.h"

static int success, fail, total_success, total_fail;
static void (*clear_elements)(void);

void
BEGIN_TESTING(void (*clear)(void)) {
  printf("Start testing...\n\n");
  total_success = total_fail = 0;

  clear_elements = clear;
}

int
FINISH_TESTING(void) {
  printf("\n%d tests ended: %d successes, %d fails\n",
      total_success + total_fail, total_success, total_fail);
  return total_fail;
}

void
cunit_start_test(const char *func) {
  printf("Start %s\n", func);
  if (clear_elements) {
    clear_elements();
  }
  success = 0;
  fail = 0;
}

void
cunit_end_test(const char *func) {
  printf("End %s: %d successes, %d fails\n", func, success, fail);
  total_success += success;
  total_fail += fail;
}

void
cunit_named_check(bool cond, const char *name, int line, const char *format, ...) {
  va_list ap;

  if (cond) {
    success++;
    return;
  }

  fail++;

  va_start(ap, format);

  printf("\t%s (%d) fail: ", name, line);
  vprintf(format, ap);
  puts("\n");
  va_end(ap);
}
