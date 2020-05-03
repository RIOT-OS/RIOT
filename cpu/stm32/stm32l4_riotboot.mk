# "The Vector table must be naturally aligned to a power of two whose alignment
# value is greater than or equal to number of Exceptions supported x 4"
# CPU_IRQ_NUMOFF for stm32l4 boards is < 91+16 so (107*4 bytes = 428 bytes ~= 0x200)
# RIOTBOOT_HDR_LEN can be set to 0x200
RIOTBOOT_HDR_LEN ?= 0x200
