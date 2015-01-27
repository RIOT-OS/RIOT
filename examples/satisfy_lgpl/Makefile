# name of your application
APPLICATION = satisfy_lgpl

# Name of your propriatary library
LIBNAME = my_cool_lib

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Include path to your library headers
CFLAGS += -I$(CURDIR)

# Use the build library when linking if PROPRIETARY_BUILD is set
ifneq (,$(PROPRIETARY_BUILD))
	BASELIBS += $(CURDIR)/$(LIBNAME).a
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# An additional module used from the library to demonstrate the integration/interaction
USEMODULE += base64

include $(RIOTBASE)/Makefile.include

# Indicates if the library source is excluded from the build
LIBSOURCE_OFF = $(wildcard $(CURDIR)/library.c_off)

# Target to apply the provided patch and create a revert patch
apply_patch:
ifeq ($(wildcard $(CURDIR)/mutex_revert.patch),)
	cp ../../core/mutex.c mutex_bak.c
	patch ../../core/mutex.c < mutex_apply.patch
	diff -u ../../core/mutex.c mutex_bak.c > mutex_revert.patch || exit 0
	rm mutex_bak.c
endif

# Target to apply the revert patch if available and finally removing it
revert_patch:
ifneq ($(wildcard $(CURDIR)/mutex_revert.patch),)
	patch ../../core/mutex.c < mutex_revert.patch
	rm mutex_revert.patch
endif

# Target to include the library source to the build directly, used for testing
use_source: apply_patch
ifneq ($(LIBSOURCE_OFF),)
	mv library.c_off library.c
endif

# Target to exclude the library source from the build, used for releasing the library
use_library: apply_patch
ifeq ($(LIBSOURCE_OFF),)
	mv library.c library.c_off
endif

# Target to create the library
create_library: use_source clean all
ifneq ($(wildcard $(CURRDIR)/$(LIBNAME).a),)
	rm $(CURRDIR)/$(LIBNAME).a
endif
	ar -rcs $(CURDIR)/$(LIBNAME).a  $(CURDIR)/bin/$(BOARD)/$(APPLICATION)/library.o

# Target to build your application with the source, used for developing/testing
with_source: use_source clean all

# Target to build your application with the library,
# use this target with PROPRIETARY_BUILD=1 to link to the library
with_library: use_library clean all

# Target to revert the changes and do some housekeeping
unmodify: use_source revert_patch clean
