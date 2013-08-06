ifeq ($(CPU),lpc2387)
 DIRS = arm_common lpc_common lpc2387
endif

ifeq ($(CPU),lpc214x)
 DIRS = arm_common lpc_common lpc214x
endif

ifeq ($(CPU),mc1322x)
	DIRS = arm_common mc1322x
endif

ifeq ($(CPU),cc430)
 DIRS = msp430-common cc430
endif

ifeq ($(CPU),msp430x16x)
 DIRS = msp430-common msp430x16x
endif

ifeq ($(CPU),native)
 DIRS = native
endif

.PHONY: cpus
.PHONY: $(DIRS)

cpus: $(DIRS)

$(DIRS): 
	@"$(MAKE)" -C $@

clean:
	@"$(MAKE)" -C lpc2387 clean
	@"$(MAKE)" -C mc1322x clean
	@"$(MAKE)" -C lpc_common clean
	@"$(MAKE)" -C arm_common clean
	@"$(MAKE)" -C cc430 clean
	@"$(MAKE)" -C msp430-common clean
	@"$(MAKE)" -C msp430x16x clean
	@"$(MAKE)" -C native clean
