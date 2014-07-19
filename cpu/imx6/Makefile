MODULE = cpu

export DIRS = variant\
	periph\
	core

.PHONY: variant_dir core_dir link.lds cpu_default

cpu_default: core_dir variant_dir link.lds all

include $(RIOTBASE)/Makefile.base

variant_dir:
	rm -f variant
	ln -s $(IMX6_SDK_PATH)/$(IMX6_VARIANT) variant
	#ln -s $(RIOTBASE)/pkg/imx6variants/$(IMX6_VARIANT) variant

core_dir:
	rm -f core
	ln -s $(IMX6_SDK_PATH)/MX6CORE core
	#ln -s $(RIOTBASE)/pkg/imx6variants/$(IMX6_VARIANT) variant


link.lds:
	cp link.lds.raw link.lds.S
	$(CC) -E $(CFLAGS) $(INCLUDES) link.lds.S | sed '/^#/d' > link.lds
	rm link.lds.S

# TODO this doesn't seem to be called..
clean::
	rm -f link.lds variant core
