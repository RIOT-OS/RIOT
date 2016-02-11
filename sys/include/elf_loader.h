/*
 * Copyright (C) 2015  Kushal Singh <kushal.spiderman.singh@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_elf_loader
 * @{
 *
 * @file
 * @brief       Implementation of a dynamic elf linker
 *
 * @author      Kushal Singh <kushal.spiderman.singh@gmail.com>
 *
 * @}
 */

#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include "elf.h"


/**
 * @brief Prints the section table entries in elf
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 */
void list_shdr_table_entries(char *elf_ptr);

/**
 * @brief Gives the symbols details and perfoms relocation based on symbol table
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 */
void list_symbol_info(char * elf_ptr);

/**
 * @brief Prints the section associated with the symbol
 *
 * @param[in] shndx	Section Index of the symbol
 */
void print_symbol_info(Elf32_Half shndx);

/**
 * @brief Process the symbol table to give symbol name and it's section type
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] sym_index Index of symbol table
 */
void process_symtab(char* elf_ptr, int sym_index);

/**
 * @brief Print the type of relocation (Currently a subset of all possible)
 *
 * @param[in] r_info Gives information about relcoation type
 */
void print_rel_type(Elf32_Word r_info);

/**
 * @brief Print the value and name of the symbol in the symtab table
 *
 * @param[in] elf_ptr 		Pointer to the elf file
 * @param[in] symtab_index 	Index of the symbol table associated with symbol
 * @param[in] strtab_index 	Index of the string table associated with symbol
 */
void print_sym_name(char * elf_ptr, int symtab_index, int strtab_index);

/**
 * @brief Print the symbol related relocation info from the relocation
 		  table (currently .rel only)
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] rel_index Index of relocation table
 */
void process_rel(char * elf_ptr, int rel_index);

/**
 * @brief Process the symbol table to locate the file_ptr address
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] file_ptr 	Name of the file to search for
 */
Elf32_Addr elf_locate(char * elf_ptr, char *file_ptr);

/**
 * @brief Returns pointer to a section
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] index 	Offset of the section
 */
Elf32_Shdr* getElfSectionHdr(char *elf_ptr, int index);

/**
 * @brief Returns a pointer to a symbol from symbol table
 *
 * @param[in] elf_ptr 		Pointer to the elf file
 * @param[in] sh_offset 	Offset of the section
 * @param[in] index 		Index in the symbol table
 */
Elf32_Sym* getElfSym(char *elf_ptr, Elf32_Off sh_offset, int index);

/**
 * @brief Returns a pointer to relocation table symbol
 *
 * @param[in] elf_ptr 		Pointer to the elf file
 * @param[in] sh_offset 	Offset of the relocation section
 * @param[in] index 		Index in the relocation table
 */
Elf32_Rel* getElfRel(char *elf_ptr, Elf32_Off sh_offset, int index);

/**
 * @brief Returns the actual symbol value(relocatd) of a function
 *
 * @param[in] elf_ptr 	    Pointer to the elf file
 * @param[in] symtab_index	Index to the symbol table
 * @param[in] index 		Index of the symtab entry
 */
Elf32_Addr getElfSymval(char * elf_ptr, int symtab_index, int index);

/**
 * @brief Perform the relocations in the elf file at the needed locations given by the offset
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 */
int elf_parse_rel(char * elf_ptr, int rel_index);

/**
 * @brief Calls elf_modify_instruction and calls for processing symbol table.
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 */
void elf_relocate_internal(char *elf_ptr);

/**
 * @brief Populates struct to be used with external symbols
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] sym_index Index of symbol table
 */
void elf_process_symtab(char *elf_ptr, int sym_index);

/**
 * @brief Computes the immediate operand for movw/movt instruction
 *
 * @param[in] reg 	Address of the register
 */
Elf32_Addr compute_imd(Elf32_Addr reg);

/**
 * @brief Compute the address of the got entry from plt entries
 *
 * @param[in] plt_jmp 	Address of the plt entry
 */
Elf32_Addr elf_jmp_got( Elf32_Addr* plt_jmp);

/**
 * @brief Modifies the branch instruction.
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 */
void elf_modify_instruction(char *elf_ptr);

/**
 * @brief Perform the relocations in the elf file at the needed locations
 *		  given by the offset
 *
 * @param[in] elf_ptr 	Pointer to the elf file
 * @param[in] rel_index Index to relocation table
 */
int elf_parse_rel(char * elf_ptr, int rel_index);

#endif /* ELF_LOADER_H */
