/**
 * @addtogroup loader
 * @{
 */

/**
 * This file is an adoptation of the The Contiki ELF loader.
 *
 * The ELF loader links, relocates, and loads ELF (Executable Linkable
 * Format) object files into a running RIOT system.
 *
 * ELF is a standard format for relocatable object code and executable
 * files. ELF is the standard program format for Linux, Solaris, and
 * other operating systems.
 *
 * An ELF file contains is either a standalone executable program or a
 * program module. The file contains both the program code, the
 * program data, as well as information about how to link, relocate,
 * and load the program into a running system.
 *
 * An ELF file is composed of a set of sections. The sections contain
 * program code, data, or relocation information, but can also contain
 * debugging information.
 *
 * To link and relocate an ELF file, the ELF loader first parses the
 * ELF file structure to find the appropriate ELF sections. It then
 * rewrites the ELF file, such that the blank spots are filled with
 * the correct addresses and thus making it ready for execution.
 *
 * @{
 */

/**
 * @file
 *         Header file for the Contiki ELF loader.
 * @author
 *         Adam Dunkels <adam@sics.se> (Contiki)
 *
 * Adopted for RIOT:
 *
 * @author      Simon Gene Gottlieb <s.gottlieb@fu-berlin.de>
 * @author      Dimitri Schachmann <d.schachmann@fu-berlin.de>
 * @author      Ron Wenzel <ron.wenzel@fu-berlin.de>
 * @author      Christopher Pockrandt <christopher.pockrandt@fu-berlin.de>
 *
 */

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
#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <stdint.h>

/**
 * Return value from elfloader_load() indicating that loading worked.
 */
#define ELFLOADER_OK                  0
/**
 * Return value from elfloader_load() indicating that the ELF file had
 * a bad header.
 */
#define ELFLOADER_BAD_ELF_HEADER      1
/**
 * Return value from elfloader_load() indicating that no symbol table
 * could be found in the ELF file.
 */
#define ELFLOADER_NO_SYMTAB           2
/**
 * Return value from elfloader_load() indicating that no string table
 * could be found in the ELF file.
 */
#define ELFLOADER_NO_STRTAB           3
/**
 * Return value from elfloader_load() indicating that the size of the
 * .text segment was zero.
 */
#define ELFLOADER_NO_TEXT             4
/**
 * Return value from elfloader_load() indicating that a symbol
 * specific symbol could not be found.
 *
 * If this value is returned from elfloader_load(), the symbol has
 * been copied into the elfloader_unknown[] array.
 */
#define ELFLOADER_SYMBOL_NOT_FOUND    5
/**
 * Return value from elfloader_load() indicating that one of the
 * required segments (.data, .bss, or .text) could not be found.
 */
#define ELFLOADER_SEGMENT_NOT_FOUND   6
/**
 * Return value from elfloader_load() indicating that no starting
 * point could be found in the loaded module.
 */
#define ELFLOADER_NO_STARTPOINT       7

/**
 * A pointer to the processes loaded with elfloader_load().
 */
typedef int (*process_t)(void);

/**
 * @brief      Load and relocate an ELF file.
 * @param objPtr
 *             A pointer to the object file. This pointer needs to be
 *             aligned according to platform flash
 *             requirements. Usually word size.

 * @param entry_point_name
 *             Name of the symbol for wich the address is written to
 *             elfloader_process parameter.
 *
 * @return     ELFLOADER_OK if loading and relocation worked.
 *             Otherwise an error value.
 *
 *             This function loads and relocates an ELF file.
 *
 *             If the function is able to load the ELF file, a pointer
 *             to the entry_point_name is written to elfloader_process.
 *
 * @note
 *             This function modifies the ELF file.  If the contents
 *             of the file is required to be intact, the file must be
 *             backed up first.
 */
int elfloader_load(void * objPtr, const char * entry_point_name, process_t** elfloader_process);

typedef uint32_t elf32_word;
typedef int32_t  elf32_sword;
typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef uint32_t elf32_addr;

/* representation of a relocation */
typedef struct elf32_rela {
	elf32_addr      offset;       /* location of the relocation, as an
								     offset to section start. This is
								     where the new address needs to be
								     written. */

	elf32_word      info;         /* Relocation type and symbol index. */

	elf32_sword     addend;       /* Addend */
} elf32_rela_t;


#endif /* ELFLOADER_H_ */

/** @} */
/** @} */
