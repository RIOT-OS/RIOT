PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://tlsf.baisoku.org/$(PKG_FILE)

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif

.PHONY: all clean patch reset

all: $(CURDIR)/$(PKG_NAME)/Makefile
	make -C $(CURDIR)/$(PKG_NAME)

$(CURDIR)/$(PKG_NAME)/Makefile: $(CURDIR)/$(PKG_NAME)/
	cd $(CURDIR)/$(PKG_NAME) && patch -p0 -N -i ../patch.txt

$(CURDIR)/$(PKG_NAME)/: $(CURDIR)/$(PKG_FILE)
	@rm -rf $(CURDIR)/$(PKG_NAME)/ && mkdir -p $(CURDIR)/$(PKG_NAME)/
	cd $(CURDIR)/$(PKG_NAME) && 7z x $(CURDIR)/$(PKG_FILE)

$(CURDIR)/$(PKG_FILE):
	wget -nv -c -O $(CURDIR)/$(PKG_FILE) $(PKG_URL)

clean::
	rm -rf $(CURDIR)/$(PKG_FILE)

distclean:: clean
	rm -f $(CURDIR)/$(PKG_FILE)
