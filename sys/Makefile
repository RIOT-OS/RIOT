INCLUDES += -Iinclude -I$(RIOTBASE)/drivers/ -I$(RIOTBASE)/drivers/include -Ilib -I$(RIOTCPU)/$(CPU)/include -Inet -I../core/include

MODULE =sys
ifneq (,$(findstring auto_init,$(USEMODULE)))
    DIRS += auto_init
endif
ifneq (,$(findstring config,$(USEMODULE)))
    DIRS += config
endif
ifneq (,$(findstring lib,$(USEMODULE)))
    DIRS += lib
endif
ifneq (,$(findstring cmdd,$(USEMODULE)))
    DIRS += cmdd
endif
ifneq (,$(findstring mprint,$(USEMODULE)))
    DIRS += mprint
endif
ifneq (,$(findstring ping,$(USEMODULE)))
    DIRS += ping
endif
ifneq (,$(findstring ps,$(USEMODULE)))
    DIRS += ps
endif
ifneq (,$(findstring sync_read,$(USEMODULE)))
    DIRS += sync_read
endif
ifneq (,$(findstring sysmon,$(USEMODULE)))
    DIRS += sysmon
endif
ifneq (,$(findstring mqueue,$(USEMODULE)))
    DIRS += mqueue
endif
ifneq (,$(findstring posix,$(USEMODULE)))
    DIRS += posix
endif
ifneq (,$(findstring shell,$(USEMODULE)))
    DIRS += shell
endif
ifneq (,$(findstring shell_commands,$(USEMODULE)))
    DIRS += shell/commands
endif
ifneq (,$(findstring timex,$(USEMODULE)))
    DIRS += timex
endif
ifneq (,$(findstring transceiver,$(USEMODULE)))
    DIRS += transceiver
endif
ifneq (,$(findstring uart0,$(USEMODULE)))
    DIRS += uart0
endif
ifneq (,$(findstring vtimer,$(USEMODULE)))
    DIRS += vtimer
endif
ifneq (,$(findstring destiny,$(USEMODULE)))
    DIRS += net/destiny
endif
ifneq (,$(findstring net_mm,$(USEMODULE)))
    DIRS += net/mm
endif
ifneq (,$(findstring net_help,$(USEMODULE)))
    DIRS += net/net_help
endif
ifneq (,$(findstring protocol_multiplex,$(USEMODULE)))
    DIRS += net/protocol-multiplex
endif
ifneq (,$(findstring sixlowpan,$(USEMODULE)))
    DIRS += net/sixlowpan
endif
ifneq (,$(findstring rpl,$(USEMODULE)))
    DIRS += net/rpl
endif
ifneq (,$(findstring ieee802154,$(USEMODULE)))
    DIRS += net/ieee802154
endif
ifneq (,$(findstring bloom,$(USEMODULE)))
    DIRS += bloom
endif
ifneq (,$(findstring crypto_3des,$(USEMODULE)))
    DIRS += crypto/3des
endif
ifneq (,$(findstring crypto_aes,$(USEMODULE)))
    DIRS += crypto/aes
endif
ifneq (,$(findstring crypto_rc5,$(USEMODULE)))
    DIRS += crypto/rc5
endif
ifneq (,$(findstring crypto_sha256,$(USEMODULE)))
    DIRS += crypto/sha256
endif
ifneq (,$(findstring crypto_skipjack,$(USEMODULE)))
    DIRS += crypto/skipjack
endif
ifneq (,$(findstring crypto_twofish,$(USEMODULE)))
    DIRS += crypto/twofish
endif
ifneq (,$(findstring random,$(USEMODULE)))
    DIRS += random
endif
ifneq (,$(findstring hashes,$(USEMODULE)))
    DIRS += hashes
endif
ifneq (,$(findstring semaphore,$(USEMODULE)))
    DIRS += semaphore
endif
ifneq (,$(findstring ccn_lite,$(USEMODULE)))
    DIRS += net/ccn_lite
endif
ifneq (,$(findstring ccn_lite_client,$(USEMODULE)))
    DIRS += net/ccn_lite/util
endif

all: $(BINDIR)$(MODULE).a 
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

# remove compilation products
clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

