PKG_NAME     = mosquitto_rsmb
PKG_URL      = https://github.com/eclipse/mosquitto.rsmb
PKG_VERSION  = 9b99a3be9a26635b93aec8fa2ed744e8c49e7262
PKG_LICENSE  = EPL-1.0
PKG_BUILDDIR = $(CURDIR)/bin

# set default configuration file
RSMB_CFG ?= $(CURDIR)/config.cnf

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..
GITCACHE ?= $(RIOTTOOLS)/git/git-cache

$(info $(RIOTBASE))

.PHONY: all clean

all: git-download
# Start mosquitto_rsmb build in a clean environment, so variables set by RIOT's
# build process for cross compiling a specific target platform are reset and
# mosquitto_rsmb can be built cleanly for the host platform.
	env -i PATH=$(PATH) TERM=$(TERM) "$(MAKE)" -C $(PKG_BUILDDIR)/rsmb/src
	cp $(PKG_BUILDDIR)/rsmb/src/broker_mqtts $(CURDIR)/mosquitto_rsmb
	cp $(PKG_BUILDDIR)/rsmb/src/Messages.1.* $(CURDIR)/

run:
	@$(CURDIR)/mosquitto_rsmb $(RSMB_CFG)

clean::
	@rm -rf $(CURDIR)/bin
	@rm -f $(CURDIR)/mosquitto_rsmb
	@rm -f $(CURDIR)/Messages.1.*
	@rm -f $(CURDIR)/FFDC.CWNAN.*.dmp

include $(RIOTBASE)/pkg/pkg.mk
