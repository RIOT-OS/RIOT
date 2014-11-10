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
#ifndef SYMBOLS_DEF_H_
#define SYMBOLS_DEF_H_

// represents a symbol in an object file
struct symbols {
	const char *name; // name of the symbol
	void *value;      // address of the symbol at runtime
};

/**
 * This variable holds the number of symbols in RIOT. For definition,
 * please look below at symbols[] declaration documentation.
 */
extern const int symbols_nelts;

/**
 * This array holds all the symbols in RIOT.
 *
 * The definition of this array needs to be generated externally by
 * parsing a compiled RIOT object with this array being empty. From
 * this a symbols.c file with the correct symbols in this array is
 * generated, but since the size of the array changed, the addresses
 * of all symbols after this array are wrong, so another compile pass
 * is needed, generating a symbols.c, that defines all the symbols in
 * RIOT with the correct addresses.
 *
 * Will have symbols_nelts size.
 */
extern const struct symbols symbols[];

#endif /* SYMBOLS_DEF_H_ */
