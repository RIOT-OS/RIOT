connect arm hw
rst

source cache/ws/zed_hw_platform/ps7_init.tcl

ps7_init
ps7_post_config

#puts bin/$::env(BOARD)/$::env(APPLICATION).elf
dow bin/$::env(BOARD)/$::env(APPLICATION).elf
#dow bin/$(BOARD)/$(APPLICATION).elf
con
