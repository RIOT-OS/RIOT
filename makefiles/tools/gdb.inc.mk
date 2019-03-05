# new versions of gdb will support all architectures in one binary
ifeq ($(shell gdb-multiarch -v 2>&1 > /dev/null; echo $$?),0)
export GDB        ?= gdb-multiarch
else
export GDBPREFIX  ?= $(PREFIX)
export GDB        ?= $(GDBPREFIX)gdb
endif
