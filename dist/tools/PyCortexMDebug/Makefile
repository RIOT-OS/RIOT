PKG_NAME     = PyCortexMDebug
PKG_URL      = https://github.com/bnahill/PyCortexMDebug
PKG_VERSION  = ce371500500a9168729f04b9dcad5f84440deadd
PKG_LICENSE  = GPL-3.0-only

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/checkout
PKG_BUILD_OUT_OF_SOURCE := 0

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all
all: prepare
