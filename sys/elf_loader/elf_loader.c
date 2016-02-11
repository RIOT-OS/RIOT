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

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "elf_loader.h"

typedef struct rel_array{
    char *name;
    Elf32_Addr addr;
}rel_array;

rel_array r_array[100];
int count;

Elf32_Shdr* getElfSectionHdr(char *elf_ptr, int index)
{return (Elf32_Shdr *) (elf_ptr + ((Elf32_Ehdr *) elf_ptr)->e_shoff + index *((Elf32_Ehdr *) elf_ptr)->e_shentsize);}

Elf32_Sym* getElfSym(char *elf_ptr, Elf32_Off sh_offset, int index)
{return (Elf32_Sym *) (elf_ptr + sh_offset + index * sizeof(Elf32_Sym));}

Elf32_Rel* getElfRel(char *elf_ptr, Elf32_Off sh_offset, int index)
{return (Elf32_Rel *) (elf_ptr + sh_offset + index * sizeof(Elf32_Rel));}

/* Matches if current section name is same as given shname parameter */
int match_section(char *elf_ptr, char *shname, int shndx)
{
    for(int index = 0; index < ((Elf32_Ehdr *)elf_ptr)->e_shnum ; index++)
    {
        Elf32_Shdr *shdr= (Elf32_Shdr *) getElfSectionHdr(elf_ptr, index);
        Elf32_Shdr *strhdr = (Elf32_Shdr *) getElfSectionHdr(elf_ptr, ((Elf32_Ehdr *)elf_ptr)->e_shstrndx);

        if((strcmp(elf_ptr + strhdr->sh_offset + shdr->sh_name, shname)>=0) && (index == shndx))
   			{printf("%s\n"	,elf_ptr + strhdr->sh_offset + shdr->sh_name);return index;}
    }

    return 0;
}

/* Populates struct to be used with external symbols in order to provide functionality 
   for a new section the this function can be used with a symbol table */
void elf_process_symtab(char *elf_ptr, int sym_index)
{
    Elf32_Shdr *shdr = (Elf32_Shdr *) getElfSectionHdr(elf_ptr, sym_index);
    int num_entries = shdr->sh_size/sizeof(Elf32_Sym);

    for(int index = 0; index < num_entries ; index++)
    {
        Elf32_Sym *symbol = (Elf32_Sym *) getElfSym(elf_ptr, shdr->sh_offset, index);
            
        /* Add condition for weak symbol later (if needed) */
        if((ELF32_ST_TYPE(symbol->st_info) == STT_FUNC) && (ELF32_ST_BIND(symbol->st_info) == STB_GLOBAL) \
            && (ELF32_ST_BIND(symbol->st_info) == STB_LOCAL)){

        		if(match_section(elf_ptr,".text",symbol->st_shndx)){
	                r_array[count].name = elf_ptr + getElfSectionHdr(elf_ptr, shdr->sh_link)->sh_offset + symbol->st_name;
	                r_array[count].addr = symbol->st_value;
	                count+=1;
            	}
        }

    }
}

/* The instruction format is similar for two instruction movw and movt
	MOVW<c> <Rd>,#<imm16>/MOVT<c> <Rd>,#<imm16>
	Calculate the immediate operand imm32 = (imm4:i:imm3:imm8, 32);
	
	For details on intructions view - (https://web.eecs.umich.edu/~prabal/teaching/eecs373-f11/readings/ARMv7-M_ARM.pdf)
 */
Elf32_Addr compute_imd(Elf32_Addr reg)
{
	Elf32_Addr imm8 = (0xFF & (reg));
	Elf32_Addr imm3 = (0x7000 & (reg))>>12;
	Elf32_Addr i = ((1<< 27) & (reg))>>26;
	Elf32_Addr imm4 = ((0xF0000) & (reg))>>16;
	return ((imm4<<12) + (i<<11) + (imm3<<8) + (imm8));
}

/* Process the two instruction movw and movt which are present in a sequential order.
   Get the immediate operands from both the instruction and combine them to get the
   index for the corresponding entry in the global offset table
*/
Elf32_Addr elf_jmp_got( Elf32_Addr* plt_jmp)
{
	/* Getting last 16 bits from movw instruction */
	Elf32_Addr movw_imm = compute_imd(((*plt_jmp) & (0xFFFF0000))>>16) + (((*plt_jmp) & (0xFFFF))<<16);
	plt_jmp+=1;

	/* Getting first 16 bits from the movw instruction */
	Elf32_Addr movt_imm = compute_imd(((*plt_jmp) & (0xFFFF0000))>>16) + (((*plt_jmp) & (0xFFFF))<<16);
	plt_jmp+=1;
	/* Add pc+4 to the current value so make address pc relative*/
	return ((movt_imm<<16)|((movw_imm)&(0xFFFF))) + (Elf32_Addr)0x4 + (Elf32_Addr)plt_jmp;

}

/* Cortex-M0/M0+ does not currently support movw and movt instructions (resulting in hard faults).
   See the bug- (https://sourceware.org/bugzilla/show_bug.cgi?id=16017)

   In order to bypass it. We try to replace the address of the jump instruction to plt by the 
   relocated address. For each .text section present in the elf look for all the bl instruction
   that go to .plt section which in turn point to .got and relocate the address to point directly
   to the function. This also helps to reduce overhead for the same.

*/
void elf_modify_instruction(char *elf_ptr)
{
	for( int index=0; index < ((Elf32_Ehdr *)elf_ptr)->e_shnum; index++){
	    /*Look for matching .text section in the file*/
	    int text_idx = match_section(elf_ptr,".text",index);
	    
	    if(text_idx){

		    Elf32_Shdr* shdr = getElfSectionHdr(elf_ptr,text_idx);
		    Elf32_Half* start = (Elf32_Half *)(elf_ptr + shdr->sh_offset);
		    Elf32_Addr jump_addr=0;
		    while(start < (Elf32_Half *)(elf_ptr + shdr->sh_offset + shdr->sh_size))
		    {
		    	/* The instruction format for bl can be found here - https://ece.uwaterloo.ca/~ece222/ARM/ARM7-TDMI-manual-pt3.pdf */
		    	/* Checks if the current instruction is a bl instructin */
		        if(!(((char)0xf)^((*start)>>12))){

		        	 /* Check if the current block has high bit set for the instruction */
		        	 if(((char)1)&((*start)>>11)){

	        	 		/* addr += (offset)<<1 */
						jump_addr += (((*start)&((Elf32_Half)0x7FF))<<1);

						/* Compute the address by which to replace in the .got.plt section */
						Elf32_Addr my_got = elf_jmp_got((Elf32_Addr *)jump_addr);

						/*Calculate the high bits */
						Elf32_Half bl_high_addr =(((*start)>>11)<<11)|(((Elf32_Half)(((*(Elf32_Addr *)my_got - (Elf32_Addr)(start) - (Elf32_Addr)(0x4))))>>1)&(0x7FF));
						memcpy(start,&bl_high_addr,sizeof(Elf32_Half));
						start--;

						/*Calculate the low bits */
						Elf32_Half bl_low_addr =((Elf32_Half)((Elf32_Half)(*start)>>11)<<11)|(((Elf32_Addr)(((*(Elf32_Addr *)my_got - (Elf32_Addr)(start))))>>12)&(0x7FF));
						memcpy(start,&bl_low_addr,sizeof(Elf32_Half));
						start++;
						jump_addr=0;

					} else{
						/* addr+= PC + (offset)<<12 */
						jump_addr += (Elf32_Addr)(start) + (Elf32_Addr)0x4;
						jump_addr += ((*start)&((Elf32_Half)0x7FF))<<12;

						printf("%"PRIx16"\n", (unsigned)start);
							printf("High Part+Low+PC %"PRIx32"\n",(long unsigned)jump_addr);
					}
		        }
		        start++;
		    }
		}
	}
}

/* Calls the elf_process_symtab (currently not useful) and then sets on to modify instruction */
void elf_relocate_internal(char *elf_ptr)
{
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_ptr;
    for(int index = 0; index < ehdr->e_shnum; index++)
    {
        Elf32_Shdr *shdr = (Elf32_Shdr *) getElfSectionHdr(elf_ptr, index);
        if(shdr->sh_type == SHT_DYNSYM) 
            elf_process_symtab(elf_ptr, index);
    }
    elf_modify_instruction(elf_ptr);
}
/* Gives the symbols details and perfoms relocation based on symbol table*/
void list_symbol_info(char * elf_ptr)
{
    Elf32_Ehdr *elfHeader;
    Elf32_Shdr *sectionHeader;

    elfHeader = (Elf32_Ehdr *) elf_ptr;

    /* Finding index of position of symtab */
    for(int index = 0; index < elfHeader->e_shnum; index++)
    {
        sectionHeader = getElfSectionHdr(elf_ptr, index);
                printf("Off=%x ",(unsigned)(elf_ptr + sectionHeader->sh_offset));

        printf("index=%u sh_type=0x%08x\n", index, (unsigned)sectionHeader->sh_type);

        if(sectionHeader->sh_type == SHT_DYNSYM) {
            process_symtab(elf_ptr, index);
        }
        else if (sectionHeader->sh_type == SHT_REL) {
            printf("Return value - %d\n",elf_parse_rel(elf_ptr, index));
        }
    }

}

/* Prints the section associated with the symbol */
void print_symbol_info(Elf32_Half shndx)
{
    if( shndx == SHN_UNDEF )
        printf(" -  Undefined Symbol  ");
    else if( shndx == SHN_COMMON)
        printf(" -  COMMON (Currently Unallocated)  ");
    else if (shndx == SHN_ABS)
        printf(" -  ABS (No relocation)  ");
    else
        printf(" -  %s  ","section based"); //section_name[shndx](TODO)
}

/* Process the symbol table to give symbol name and it's section type */
void process_symtab(char* elf_ptr, int sym_index)
{
    char *stringTable = NULL;
    Elf32_Shdr *sectionHeader, *strtabHeader;
    Elf32_Sym *symbol_symtab;
    int num_entries;

    /* Jumping to the symtab entry directly */
    sectionHeader = getElfSectionHdr(elf_ptr, sym_index);

    /* Reading from the String Table linked to ot */
    strtabHeader = getElfSectionHdr(elf_ptr, sectionHeader->sh_link);

    stringTable = elf_ptr + strtabHeader->sh_offset;

    num_entries = sectionHeader->sh_size/sizeof(Elf32_Sym);
    printf("Printing Symtab entries\n");
    printf("Idx  Sym_name  Section/Type\n");

    for(int index = 0; index < num_entries ; index++)
    {
        symbol_symtab = getElfSym(elf_ptr, sectionHeader->sh_offset, index);

        if(index < 10)
            printf("( %d) - %s ", index, stringTable + symbol_symtab->st_name);
        else
            printf("(%d) - %s ", index, stringTable + symbol_symtab->st_name);

        print_symbol_info(symbol_symtab->st_shndx);
        printf("\n");
    }

}

/* Print the value and name of the symbol in the symtab table (can be coded in more optimized way */
void print_sym_name(char * elf_ptr, int symtab_index, int strtab_index)
{
    Elf32_Shdr *sectionHeader, *strtabHeader;
    Elf32_Sym *symbol_symtab;
    char *stringTable;

    sectionHeader = getElfSectionHdr(elf_ptr, symtab_index);

    /* Reading from the String Table linked to it */
    strtabHeader = getElfSectionHdr(elf_ptr, sectionHeader->sh_link);

    stringTable = elf_ptr + strtabHeader->sh_offset;

    if(strtab_index >= strtabHeader->sh_size){
        printf("  Out of Bounds ");
        return ;
    }

    /* Moving to the Symbol table data */
    symbol_symtab = getElfSym(elf_ptr, sectionHeader->sh_offset,strtab_index);

    if( symbol_symtab->st_shndx == SHN_UNDEF )
        printf(" -  Undefined Symbol  ");
    else if( symbol_symtab->st_shndx == SHN_COMMON)
        printf(" -  COMMON (Currently Unallocated)  ");
    else if (symbol_symtab->st_shndx == SHN_ABS)
        printf(" -  ABS (No relocation)  ");
    else
        printf(" - ** %d ** ",symbol_symtab->st_shndx);

    (void)stringTable;
}

/* Print the type of relocation (Currently a subset of all possible) */
void print_rel_type(Elf32_Word r_info)
{
	/* The relocation entries can be added based on use */
    switch(ELF32_R_TYPE(r_info))
    {
        case R_386_NONE:
            printf("R_386_NONE No relocation");
            break;
        case R_386_32:
            printf("R_386_32 (Symbol + Offset)");
            break;
        case R_386_PC32:
            printf("R_386_PC32 (Symbol + Offset - Section Offset)");
            break;
        case R_386_GOT32:
            printf("R_386_GOT32 (G + A - P)");
            break;
        case R_386_PLT32:
            printf("R_386_PLT32 (L + A - P)");
            break;
        case R_386_COPY:
            printf("R_386_COPY (none)");
            break;
        case R_386_GLOB_DAT:
            printf("R_386_GLOB_DAT (S)");
            break;
        case R_386_JMP_SLOT:
            printf("R_386_JMP_SLOT (S)");
            break;
        case R_386_RELATIVE:
            printf("R_386_RELATIVE (B + A)");
            break;
        case R_386_GOTOFF:
            printf("R_386_GOTOFF (S + A - GOT)");
            break;
        case R_386_GOTPC:
            printf("R_386_GOTOFF (GOT + A - P)");
            break;
        default:
            printf("Unlisted/Unknown/Unsupported Relocation Type");// (%d)", ELF32_R_TYPE(r_info));
    }
    printf("  \n");
}

/* Returns the actual symbol value(relocatd) of a function for changing the section contents */
Elf32_Addr getElfSymval(char * elf_ptr, int symtab_index, int index)
{
    Elf32_Shdr *sectionHeader;
    Elf32_Sym *symbol;
    int symtab_entries;

    /* Get the section header info of the symbol table */
    sectionHeader = getElfSectionHdr(elf_ptr, symtab_index);

    /* Calculate the number of entries in the symbol table */
	symtab_entries = sectionHeader->sh_size / sectionHeader->sh_entsize;

    if(index >= symtab_entries){
        printf("  Index: Out of Bounds ");
        return -1;
    }

    /* Moving to the Symbol table data (symbol at the given index) */
    symbol = getElfSym(elf_ptr, sectionHeader->sh_offset,index);
	if(symbol->st_shndx == SHN_UNDEF) {
		/* External symbol, lookup value - Return zero currently. Can be implemented after adding new section */
		return 0;
	}
	else if(symbol->st_shndx == SHN_ABS) {
		/* The address of the symbol is absolute */
		return (Elf32_Addr ) symbol->st_value;
	} 
	else {
		/* Internally defined symbols - We deal with virtual addresses (shared file)*/
		return (Elf32_Addr )((Elf32_Addr)(elf_ptr) + symbol->st_value);
	}
}

/* Perform the relocations in the elf file at the needed locations given by the offset */
int elf_parse_rel(char * elf_ptr, int rel_index)
{
    Elf32_Addr sym_value;
    Elf32_Shdr *sectionHeader;
    Elf32_Rel *rel_entry;
    int num_entries, symtab_index;

    /* Jumping to the .rel table entry directly */
    sectionHeader = getElfSectionHdr(elf_ptr, rel_index);

    /* Reading the index of the Symbol Table linked to it */
    symtab_index = sectionHeader->sh_link;

    num_entries = sectionHeader->sh_size/sizeof(Elf32_Rel);

    printf("Relocation started ..\n");
	for(int index = 0; index < num_entries ; index++)
    {
        rel_entry = getElfRel(elf_ptr, sectionHeader->sh_offset, index);

        /* If symbol table index in null/not defined then take symbol value == STN_UNDEF*/
        if(ELF32_R_SYM(rel_entry->r_info) == STN_UNDEF){
           	 	printf(" STN_UNDEF   ");
            	return 1;
        } else {
		        print_rel_type(ELF32_R_TYPE(rel_entry->r_info));
				sym_value = getElfSymval(elf_ptr, symtab_index, ELF32_R_SYM(rel_entry->r_info));
			    if(sym_value == 0){
			        printf("Unable to get symbol value \n");
			        return 1;
			    }		       

			    /* Can add all the types needed based on need */
		        switch(rel_entry->r_info)
		        {
		        	/* Different types of relocations */
		        	/* Note: Right now default relocation only works */
		        	case(R_ARM_GLOB_DAT):{
		        	/* Dynamic - (S + A) | T */
			        	memcpy(elf_ptr + rel_entry->r_offset,&sym_value,sizeof(Elf32_Addr));
			        	break;
	        		}

	        		case(R_ARM_JUMP_SLOT):{
		        	/* Dynamic - (S + A) | T */
			        	memcpy(elf_ptr + rel_entry->r_offset,&sym_value,sizeof(Elf32_Addr));
			        	break;
	        		}

	        		default:
			        		memcpy(elf_ptr + rel_entry->r_offset,&sym_value,sizeof(Elf32_Addr));	        
	        	}

				sym_value = getElfSymval(elf_ptr, symtab_index, ELF32_R_SYM(rel_entry->r_info));

    			printf("%lu\n",rel_entry->r_info );
    	        print_rel_type(ELF32_R_TYPE(rel_entry->r_info));	
	        }
    }
    return 0;
}

/* Print the symbol related relocation info from the relocation table (currently .rel only) */
void process_rel(char * elf_ptr, int rel_index)
{
    Elf32_Shdr *sectionHeader;
    Elf32_Rel *rel_entry;
    int num_entries, symtab_index;

    /* Jumping to the .rel table entry directly */
    sectionHeader = getElfSectionHdr(elf_ptr, rel_index);

    /* Reading the index of the Symbol Table linked to it */
    symtab_index = sectionHeader->sh_link;

    num_entries = sectionHeader->sh_size/sizeof(Elf32_Rel);

    printf("Printing %i Relocation entries\n", num_entries);
    printf("Idx   Offset   Info    Value   Name   Sec.Indx    Type\n");

    for(int index = 0; index < num_entries ; index++)
    {
        rel_entry = getElfRel(elf_ptr, sectionHeader->sh_offset, index);
        printf("[%2d] -  %" PRIx32 "  %" PRIx32 "   ", index, rel_entry->r_offset,  rel_entry->r_info);

        /* If symbol table index in null/not defined then take symbol value == STN_UNDEF*/
        if(ELF32_R_SYM(rel_entry->r_info) == STN_UNDEF)
            printf(" STN_UNDEF   ");
        else {
    
			/* For .data section variable */
            //puts(".."); - To test relocations.
            print_sym_name(elf_ptr, symtab_index, ELF32_R_SYM(rel_entry->r_info));
            printf("\n\n%"PRIx32"\n\n",(long unsigned)elf_ptr + getElfSymval(elf_ptr, symtab_index, ELF32_R_SYM(rel_entry->r_info)));
        }
        print_rel_type(ELF32_R_TYPE(rel_entry->r_info));
    }
 }

/* Print all the Section Table Header entries */
void list_shdr_table_entries(char *elf_ptr)
{
	Elf32_Ehdr *elfHeader;
    Elf32_Shdr *sectionHeader;
    char *stringTable = NULL;

    elfHeader = (Elf32_Ehdr *) elf_ptr;
    sectionHeader = getElfSectionHdr(elf_ptr, elfHeader->e_shstrndx);

    /* Put the contents of the string table in the array */
    stringTable = elf_ptr + sectionHeader->sh_offset;

    /* Print the names of all the sections and process symbol table */
    for(int index=0; index < elfHeader->e_shnum; index++)
    {
        sectionHeader = getElfSectionHdr(elf_ptr, index);

        if(index<10)
            printf("[ %d] %s\n",index, stringTable + sectionHeader->sh_name);
        else
            printf("[%d] %s\n",index, stringTable + sectionHeader->sh_name);
    }

    printf("\n");

}


/* Process the symbol table to locate the file address. Not currently usefule for core-riot.
   Since the address of the elf is known. But this call can be easily used on linux example 
   applications. The function can be also extended to include generalized cases.
*/
Elf32_Addr elf_locate(char * elf_ptr, char *file_ptr)
{
    Elf32_Ehdr *elfHeader;
    Elf32_Shdr *sectionHeader, *strtabHeader;
    Elf32_Sym *symbol_symtab;

    char *stringTable = NULL;
    int num_entries;

    elfHeader = (Elf32_Ehdr *) elf_ptr;

    /* Finding symtab index */
    for(int index = 0; index < elfHeader->e_shnum; index++)
    {
        sectionHeader = getElfSectionHdr(elf_ptr, index);
        if(sectionHeader->sh_type == SHT_SYMTAB){

            /* Reading from the String Table linked to it */
            strtabHeader = getElfSectionHdr(elf_ptr, sectionHeader->sh_link);

            stringTable = elf_ptr + strtabHeader->sh_offset;
            num_entries = sectionHeader->sh_size/sizeof(Elf32_Sym);

            for(int i = 0; i < num_entries ; i++)
            {
                symbol_symtab = getElfSym(elf_ptr, sectionHeader->sh_offset, i);

                /* Currently we use the array test_elf[] which has type STT_OBJECT -- Maybe be changed to generalize */
                if(ELF32_ST_TYPE(symbol_symtab->st_info) == STT_OBJECT)
                {
                    if(!strcmp(file_ptr, stringTable + symbol_symtab->st_name))
                    {
                        printf("Section Index- %d  Addr Value- %" PRIx32 "\n", symbol_symtab->st_shndx,symbol_symtab->st_value);
                        return symbol_symtab->st_value;
                    }
                }

            }
        }
    }

    return 0;
}
