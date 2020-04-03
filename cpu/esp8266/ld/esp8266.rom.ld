SPI_sector_erase = 0x400040c0;
SPI_page_program = 0x40004174;
SPI_read_data = 0x400042ac;
SPI_read_status = 0x400043c8;
SPI_write_status = 0x40004400;
SPI_write_enable = 0x4000443c;
Wait_SPI_Idle = 0x4000448c;
Enable_QMode = 0x400044c0;
Disable_QMode = 0x40004508;

Cache_Read_Enable = 0x40004678;
Cache_Read_Disable = 0x400047f0;

lldesc_build_chain = 0x40004f40;
lldesc_num2link = 0x40005050;
lldesc_set_owner = 0x4000507c;

__adddf3 = 0x4000c538;
__addsf3 = 0x4000c180;
__divdf3 = 0x4000cb94;
__divdi3 = 0x4000ce60;
__divsi3 = 0x4000dc88;
__extendsfdf2 = 0x4000cdfc;
__fixdfsi = 0x4000ccb8;
__fixunsdfsi = 0x4000cd00;
__fixunssfsi = 0x4000c4c4;
__floatsidf = 0x4000e2f0;
__floatsisf = 0x4000e2ac;
__floatunsidf = 0x4000e2e8;
__floatunsisf = 0x4000e2a4;
__muldf3 = 0x4000c8f0;
__muldi3 = 0x40000650;
__mulsf3 = 0x4000c3dc;
__subdf3 = 0x4000c688;
__subsf3 = 0x4000c268;
__truncdfsf2 = 0x4000cd5c;
__udivdi3 = 0x4000d310;
__udivsi3 = 0x4000e21c;
__umoddi3 = 0x4000d770;
__umodsi3 = 0x4000e268;
__umulsidi3 = 0x4000dcf0;

bzero = 0x4000de84;
memcmp = 0x4000dea8;
memcpy = 0x4000df48;
memmove = 0x4000e04c;
memset = 0x4000e190;

strcmp = 0x4000bdc8;
strcpy = 0x4000bec8;
strlen = 0x4000bf4c;
strncmp = 0x4000bfa8;
strncpy = 0x4000c0a0;
strstr = 0x4000e1e0;

gpio_input_get = 0x40004cf0;
gpio_pin_wakeup_disable = 0x40004ed4;
gpio_pin_wakeup_enable = 0x40004e90;

ets_io_vprintf = 0x40001f00;
