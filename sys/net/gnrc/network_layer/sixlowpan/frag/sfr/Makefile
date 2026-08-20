MODULE := gnrc_sixlowpan_frag_sfr

MODULE_DEFS_NEEDED += \
  congure_abe \
  gnrc_ipv6 \
  gnrc_ipv6_nib \
  gnrc_sixlowpan_frag_sfr_congure \
  gnrc_sixlowpan_frag_sfr_ecn \
  gnrc_sixlowpan_frag_sfr_ecn_fqueue \
  gnrc_sixlowpan_frag_sfr_ecn_if_in \
  gnrc_sixlowpan_frag_sfr_ecn_if_out \
  gnrc_sixlowpan_frag_sfr_stats \
  gnrc_sixlowpan_iphc \
  gnrc_tx_sync \
  #

SRC := $(MODULE).c

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
