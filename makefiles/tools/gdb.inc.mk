export GDBPREFIX ?= $(PREFIX)

# If the user installed a magic single target GDB rather than just using
# gdb-multiarch, there typically is a reason for it - e.g. missing support for
# that particular target in gdb-multiarch.
ifeq ($(shell $(GDBPREFIX)gdb -v > /dev/null 2>&1; echo $$?),0)
  export GDB ?= $(GDBPREFIX)gdb
else
  # gdb-multiarch is normally
  export GDB ?= gdb-multiarch
endif
