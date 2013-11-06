#winheight regs 11
set history save on
set history size 1000
target remote localhost:3333
monitor reset
monitor sleep 100
monitor halt
monitor poll
#monitor arm7_9 sw_bkpts disable
#monitor arm7_9 force_hw_bkpts enable
monitor mww 0xE01FC040 0x0001
monitor mdw 0xE01FC040
monitor flash erase_sector 0 0 14
#monitor flash auto_erase on
monitor flash erase_check 0
#monitor flash write_image /home/kaspar/FeuerWhere/src/x/bin/arm.elf
set remote hardware-watchpoint-limit 2
load
break bootloader
mon soft_reset_halt
continue
d b 1
