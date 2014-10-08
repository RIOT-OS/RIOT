export APPLICATION = sixlowpan_iphc_cbuf_timeout

include ../Makefile.tests_common

USEMODULE += sixlowpan_iphc_cbuf

DISABLE_MODULE += auto_init

RANDOM_STRING = "$(shell head -c "$(1)" /dev/urandom | base64 | head -c "$(1)")"
RANDOM_U8BIT = $(shell od -vAn -N1 -tu1 < /dev/urandom | xargs echo)
RANDOM_U16BIT = $(shell od -vAn -N2 -tu2 < /dev/urandom | xargs echo)

CFLAGS += -DRAND_CID=$(shell echo "$(RANDOM_U8BIT) % 16" | bc)U
CFLAGS += -DRAND_IP="((ipv6_addr_t *)\"$(call RANDOM_STRING,16)\")"
CFLAGS += -DRAND_IP_LEN=$(shell echo "($(RANDOM_U8BIT) % 128) + 1" | bc)U

include $(RIOTBASE)/Makefile.include
