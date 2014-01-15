DIRS =
DIRS += openwsn

all::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

# remove compilation products
clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
