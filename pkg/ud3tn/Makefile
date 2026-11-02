PKG_NAME = ud3tn
PKG_URL = https://gitlab.com/d3tn/ud3tn.git
PKG_VERSION = 7b0fd79924d844ded7b75ab4611d9f24933540e6
PKG_LICENSE = Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

UD3TN_SRC_DIR = $(PKG_SOURCE_DIR)/ud3tn_platform_riot

UD3TN_COMPONENTS = ud3tn_runtime util aap aap2 agents bundle6 bundle7 cla \
                   cla_riot platform spp ud3tn aap2generated

.PHONY: all $(UD3TN_COMPONENTS)

all: $(UD3TN_COMPONENTS)

#ud3tn_runtime:
#	$(QQ)"$(MAKE)" -C $(CURDIR)/runtime

util:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/util/src

aap:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/aap

aap2:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/aap2

agents:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/agents

bundle6:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/bundle6

bundle7:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/bundle7

cla:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/cla

cla_riot:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/cla/riot

platform:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/platform/riot

spp:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/spp

ud3tn:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/ud3tn

aap2generated:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/generated/aap2
