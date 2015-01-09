# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(KINETIS_COMMON) $(CORTEX_M4_COMMON)

# Bootloader offset
CPU_BOOTLOADER_OFFSET ?= 0x0

# Run the linker script through the C preprocessor. Reason: We want to be able
# to dynamically define the bootloader offset in the RAM memory section. This is
# not possible with LD
#
# The all ..nothing directive is not only poetically interesting, it also
# serves a real purpose: As this is the first rule in the makefile, make would
# just run this rule and nothing else (default goal). By depending on the
# former default rule (all) we avoid this problem.
# ..nothing is required because otherwise the linkerscript would not build at
# all. This is because nothing depends on this rule.
$(BINDIR)k20_linkerscript.ld: all ..nothing
	$(AD)$(PREFIX)cpp -DASSEMBLY -P ld/k20.lds -I include -DCPU_BOOTLOADER_OFFSET=0xC00 -o $(BINDIR)k20_linkerscript.ld

include $(RIOTBASE)/Makefile.base

