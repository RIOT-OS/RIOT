PKG_NAME = ud3tn
PKG_URL = https://gitlab.com/d3tn/ud3tn.git
PKG_VERSION = 7b0fd79924d844ded7b75ab4611d9f24933540e6
PKG_LICENSE = Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

UD3TN_SRC_DIR = $(PKG_SOURCE_DIR)
UD3TN_RIOT_DIR = $(RIOTBASE)/pkg/ud3tn

UD3TN_COMPONENTS =  util aap aap2 agents bundle6 bundle7 cla \
                   cla_riot platform spp ud3tn aap2generated

.PHONY: all $(UD3TN_COMPONENTS)

all: $(UD3TN_COMPONENTS)


aap2:
	$(QQ)"$(MAKE)" -C $(UD3TN_RIOT_DIR)/components/aap2

cla_riot:
	$(QQ)"$(MAKE)" -C $(UD3TN_RIOT_DIR)/components/cla/riot

platform:
	$(QQ)"$(MAKE)" -C $(UD3TN_RIOT_DIR)/components/platform/riot



util:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/external/util/src \
        -f $(RIOTBASE)/Makefile.base MODULE=util

aap:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/aap \
        -f $(RIOTBASE)/Makefile.base MODULE=aap




agents:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/agents -f $(RIOTBASE)/Makefile.base MODULE=agents

bundle6:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/bundle6 -f $(RIOTBASE)/Makefile.base MODULE=bundle6

bundle7:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/bundle7 -f $(RIOTBASE)/Makefile.base MODULE=bundle7

cla:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/cla -f $(RIOTBASE)/Makefile.base MODULE=cla



spp:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/spp -f $(RIOTBASE)/Makefile.base MODULE=spp

ud3tn:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/components/ud3tn -f $(RIOTBASE)/Makefile.base MODULE=ud3tn

aap2generated:
	$(QQ)"$(MAKE)" -C $(UD3TN_SRC_DIR)/generated/aap2 -f $(RIOTBASE)/Makefile.base MODULE=aap2generated
