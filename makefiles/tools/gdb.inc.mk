# new versions of gdb will support all architectures in one binary
ifeq ($(shell gdb-multiarch -v > /dev/null 2>&1; echo $$?),0)
export GDB        ?= gdb-multiarch
else
export GDBPREFIX  ?= $(PREFIX)
export GDB        ?= $(GDBPREFIX)gdb
endif
