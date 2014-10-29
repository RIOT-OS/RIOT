/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "symtab.h"

#include "symbols.h"

#include <string.h>

/* At compile time we can choose the search algorithm for finding
   symbols: linear or binary. Binary search is twice as large but
   still small. */
#ifndef SYMTAB_CONF_BINARY_SEARCH
#define SYMTAB_CONF_BINARY_SEARCH 1
#endif

/*---------------------------------------------------------------------------*/
#if SYMTAB_CONF_BINARY_SEARCH
void *
symtab_lookup(const char *name)
{
	int start, middle, end;
	int r;

	start = 0;
	end = symbols_nelts - 1; /* Last entry is { 0, 0 }. */

	while(start <= end) {
		/* Check middle, divide */
		middle = (start + end) / 2;
		r = strcmp(name, symbols[middle].name);
		if(r < 0) {
			end = middle - 1;
		} else if(r > 0) {
			start = middle + 1;
		} else {
			return symbols[middle].value;
		}
	}
	return NULL;
}
#else /* SYMTAB_CONF_BINARY_SEARCH */
void *
symtab_lookup(const char *name)
{
	const struct symbols *s;
	for(s = symbols; s->name != NULL; ++s) {
		if(strcmp(name, s->name) == 0) {
			return s->value;
		}
	}
	return 0;
}
#endif /* SYMTAB_CONF_BINARY_SEARCH */
/*---------------------------------------------------------------------------*/
