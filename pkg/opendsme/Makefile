PKG_NAME=opendsme
PKG_URL=https://github.com/inetrg/openDSME.git
PKG_VERSION=b1969296d0fc9a1556ecbef7c0b01538dff3e10c
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

OPENDSME_MODULES = \
                    opendsme_dsmelayer \
                    opendsme_acklayer \
                    opendsme_associationmanager \
                    opendsme_beaconmanager \
                    opendsme_caplayer \
                    opendsme_gtsmanager \
                    opendsme_messagedispatcher \
                    opendsme_messages \
                    opendsme_datastructures \
                    opendsme_mcps_sap \
                    opendsme_mlme_sap \
                    opendsme_pib \
                    opendsme_adaption_layer \
                    opendsme_adaption_layer_scheduling \
					#

CPPFLAGS += -Wno-deprecated-copy -Wno-unused-parameter -Wno-error

# dsmeLayer
DIR_DSME_LAYER = dsmeLayer
DIR_opendsme_dsmelayer = $(DIR_DSME_LAYER)
DIR_opendsme_acklayer = $(DIR_DSME_LAYER)/ackLayer
DIR_opendsme_associationmanager = $(DIR_DSME_LAYER)/associationManager
DIR_opendsme_beaconmanager = $(DIR_DSME_LAYER)/beaconManager
DIR_opendsme_caplayer = $(DIR_DSME_LAYER)/capLayer
DIR_opendsme_gtsmanager = $(DIR_DSME_LAYER)/gtsManager
DIR_opendsme_messagedispatcher = $(DIR_DSME_LAYER)/messageDispatcher
DIR_opendsme_messages = $(DIR_DSME_LAYER)/messages

# MAC Services

DIR_MAC_SERVICES = mac_services
DIR_opendsme_datastructures = $(DIR_MAC_SERVICES)/dataStructures
DIR_opendsme_mcps_sap = $(DIR_MAC_SERVICES)/mcps_sap
DIR_opendsme_mlme_sap = $(DIR_MAC_SERVICES)/mlme_sap
DIR_opendsme_pib = $(DIR_MAC_SERVICES)/pib

# DSME Adoption Layer
DIR_DSME_ADAPTION_LAYER = dsmeAdaptionLayer
DIR_opendsme_adaption_layer = $(DIR_DSME_ADAPTION_LAYER)
DIR_opendsme_adaption_layer_scheduling = $(DIR_DSME_ADAPTION_LAYER)/scheduling

.PHONY: opendsme_%

export SRCXXEXT=cc

all: $(OPENDSME_MODULES)

opendsme_%:
	$(QQ)OPENDSME_MODULE=$@ "$(MAKE)" -C $(PKG_SOURCE_DIR)/$(DIR_$@) -f $(CURDIR)/Makefile.opendsme_module
