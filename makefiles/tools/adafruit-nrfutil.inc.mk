FLASHFILE = $(HEXFILE)
FLASHDEPS += $(HEXFILE).zip
FLASHER ?= adafruit-nrfutil
FFLAGS = dfu serial --port=${PORT} --baudrate=${BAUD} --touch=1200 --package=$(HEXFILE).zip --singlebank

%.hex.zip: %.hex
	$(call check_cmd,$(FLASHER),Flash program and preparation tool)
	$(FLASHER) dfu genpkg $(ADANRFUTIL_FLAGS) --application $< $@
