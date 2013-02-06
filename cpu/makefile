ifeq ($(CPU),lpc2387)
 DIRS = arm_common lpc2387
endif

ifeq ($(CPU),lpc214x)
 DIRS = arm_common lpc214x
endif

ifeq ($(CPU),cc430)
 DIRS = msp430 cc430
endif

.PHONY: cpus
.PHONY: $(DIRS)

cpus: $(DIRS)

$(DIRS): 
	@$(MAKE) -C $@

clean:
	@$(MAKE) -C lpc2387 clean
	@$(MAKE) -C arm_common clean
	@$(MAKE) -C cc430 clean
	@$(MAKE) -C msp430-common clean
	@$(MAKE) -C msp430x16x clean



