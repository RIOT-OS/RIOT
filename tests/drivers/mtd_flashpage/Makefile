include ../Makefile.drivers_common

USEMODULE += mtd_flashpage
USEMODULE += mtd_write_page
USEMODULE += embunit

FEATURES_OPTIONAL += periph_flashpage_aux

# define an auxiliary slot on the internal flash
# NOTE: This should typically be set by the board as it can not be changed in the field.
# This is only defined here for the sake of the test.
SLOT_AUX_LEN := 0x8000

include $(RIOTBASE)/Makefile.include
