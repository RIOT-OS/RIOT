PKG_NAME     = mosquitto_rsmb
PKG_URL      = https://github.com/eclipse/mosquitto.rsmb
PKG_VERSION  = 9b99a3be9a26635b93aec8fa2ed744e8c49e7262
PKG_LICENSE  = EPL-1.0

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0

# Include pkg.mk after setting RIOTBASE otherwise it is not found when this
# Makefile is called stand-alone
include $(RIOTBASE)/pkg/pkg.mk

# set default configuration file
RSMB_CFG ?= $(CURDIR)/config.cnf

all:
# Start mosquitto_rsmb build in a clean environment, so variables set by RIOT's
# build process for cross compiling a specific target platform are reset and
# mosquitto_rsmb can be built cleanly for the host platform.
	env -i PATH=$(PATH) TERM=$(TERM) "$(MAKE)" -C $(PKG_BUILD_DIR)/rsmb/src
	cp $(PKG_BUILD_DIR)/rsmb/src/broker_mqtts $(CURDIR)/mosquitto_rsmb
	cp $(PKG_BUILD_DIR)/rsmb/src/Messages.1.* $(CURDIR)/

run:
	@$(CURDIR)/mosquitto_rsmb $(RSMB_CFG)

clean::
	@rm -rf $(PKG_BUILD_DIR)
	@rm -f $(CURDIR)/mosquitto_rsmb
	@rm -f $(CURDIR)/Messages.1.*
	@rm -f $(CURDIR)/FFDC.CWNAN.*.dmp
