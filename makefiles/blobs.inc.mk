#
# ----------------------- Binary blob support ----------------------------
#
# Description
#
# This file allows applications to embed raw binary data into binaries. A
# common use case is application resources, such as images, certificates, text
# files, etc.
#
# Files are interpreted as binary and compiled into object files using `ld`.
# For each blob, a header is created with declarations.
#
# Given a file "a.b", these rules will generate "$(BINDIR)/blobs/a.b.o" and a
# header which you can include via `#include "a.b.h"`.
# The header declares three (extern) variables:
#
# - start of blob data  (points to the first memory location containing the data)
#   extern char _binary_a_b_start[];
# - end of blob data (points to a memory location just after the end of the data)
#   extern char _binary_a_b_end[];
# - size of data (in terms of number of char, like what sizeof would return).
#   size_t _binary_a_b_end; (this is actually a macro.)
#
# Usage
#
# Set the variable `BLOBS` to the files you wish to include as blobs. Access
# blob data using the include as defined above.
#
# Inner workings
#
# Blob creation proceeds in two steps. First $(LD) is used to make an object
# file from a binary. $(LD) is used because it is smarter about setting section
# properties and does not require giving being explicit about the output format.
#
# By default `ld` places the output in section ".data". Normally one wants
# blobs read-only so $(OBJCOPY) is used to rename ".data" to ".rodata". This
# renaming is configurable (see next section.)
#
# Customization
#
# The functionality here can be customized either by globally changing variables
# or on a per-file basis, by setting target specific variables. To this end,
# two helper functions are available:
#
# - blob_get_header: given the name of a blob file, generate the name of the
#                    corresponding h file.
# - blob_get_obj: given the name of a blob file, generate the name of the
#                 corresponding object file.
#
#
# - BLOB_DATATYPE: in the headers, type for the arrays in the extern declaration.
#                  Default is "const char".
# - BLOB_HDR_INCLUDES: Extra headers to include in the blob header. E.g.
#                  "<stddef.h> \"myheader.h\""
# - LD_BLOBFLAGS: flags to pass to $(LD) to create a blob from a binary.
# - BLOB_SECTION: Section where the blob should be placed (defaults to .rodata)
# - OBJCOPY_RENAMEFLAGS: flags to pass to $(OBJCOPY) when renaming sections.
#
# Advanced usage
#
# For more advanced usage, it is recommended to override rules.
#

# ############## Make app depend on blobs ################################### #

BLOB_PATH = $(BINDIR)/blobs
BLOB_IPATH = $(BLOB_PATH)/include

BLOBS_H = $(BLOBS:%=$(BLOB_IPATH)/%.h)
BLOBS_O = $(BLOBS:%=$(BLOB_PATH)/%.o)

blob_get_header = $(BLOB_IPATH)/$(1).h
blob_get_obj = $(BLOB_PATH)/$(1).h

CFLAGS += -I$(BLOB_IPATH)

BUILDDEPS += $(BLOBS_H) $(BLOB_IPATH)/
BASELIBS += $(BLOBS_O)

# ################### Create include files ################################### #

$(BLOB_PATH)/ $(BLOB_IPATH)/:
	$(Q)mkdir -p $@

blob_name_to_id = $(shell echo -n '$1' | tr -c "[:alnum:]" _)

# Important: keep the empty space below the include, it generates a newline
# after the include.
define _blob_include
	#include $1

endef

# Generate a header with extern declarations to access the blob contents.
# Parameters:
#	$1: blob_file_id
#	$2: data_type
#	$3: extra includes
#	$4: blob size
# Note: _binary_$1_size is an "absolute" symbol. Because of relocation in
# virtual memory systems (i.e. native) this will not work in such systems.
# instead, we will have to hardcode the blob size.
define blob_header_template
	/* *** Automatically generated, do not edit! *** */
	#ifndef BLOB_$1
	#define BLOB_$1

$(foreach hdr,$(3),$(call _blob_include,$(hdr)))

	extern $2 _binary_$1_start[];
	extern $2 _binary_$1_end[];
	/* Do NOT use this variable. Use _binary_$1_SIZE instead. */
	extern char _binary_$1_size;

	#if (BOARD == native)
	static const size_t _binary_$1_SIZE = $4;
	#else
	#define _binary_$1_SIZE ((size_t)&_binary_$1_size)
	#endif

	#endif $(call blob_hdr_guard_name)
	/* *** End auto-generated. ***/
endef

blob_size = $(shell wc -c <$1)

blob_header = $(call blob_header_template,$(call blob_name_to_id,$1),$2,$3,$(call blob_size,$1))
BLOB_DATATYPE ?= const char

$(BLOBS_H): $(BLOB_IPATH)/%.h: % | $(BLOB_IPATH)/
	$(file >$@,$(call blob_header,$*,$(BLOB_DATATYPE),<stddef.h> $(BLOB_HDR_INCLUDES)))

# ################### Create blobs ########################################## #

# -z noexecstack adds a .note.GNU_STACK to mark the stack as not executable.
# AFAIK this only matters in native and its absence only causes the stack to
# be marked executable in the final application binary (slight security issue).

LD_BLOBFLAGS ?= -r -b binary -z noexecstack

ifeq (native,$(BOARD))
  LD_BLOBFLAGS += -m elf_i386
endif

BLOB_SECTION ?= .rodata,alloc,load,readonly,data,contents
OBJCOPY_RENAMEFLAGS ?= --rename-section .data=$(BLOB_SECTION)

.INTERMEDIATE: $(BLOBS:%=$(BLOB_PATH)/%.o.tmp)

$(BLOBS:%=$(BLOB_PATH)/%.o.tmp): $(BLOB_PATH)/%.o.tmp: % | $(BLOB_PATH)/
	$(Q)$(LD) $(LD_BLOBFLAGS) $< -o $@

$(BLOBS_O): %: %.tmp
	$(Q)$(OBJCOPY) $(OBJCOPY_RENAMEFLAGS) $< $@
