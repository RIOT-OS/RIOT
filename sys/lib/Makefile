INCLUDES = -I../include -I../drivers/include -I../lib -I$(RIOTCPU)/$(CPU)/include -I../net -I../../core/include
MODULE =lib

ifneq (,$(findstring libcoap,$(USEMODULE)))
	DIRS += libcoap
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
