MODULE := semtech_loramac_contrib

MODULE_DEFS_NEEDED += \
  periph_eeprom \
  semtech_loramac_rx \
  sx127x \
  #

# Use -isystem instead of -I to disable warnings about these headers
INCLUDES += -isystem$(PKGDIRBASE)/semtech-loramac/src/mac \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/boards \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/radio \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/system/crypto \
            -isystem$(PKGDIRBASE)/semtech-loramac/src

include $(RIOTBASE)/Makefile.base
