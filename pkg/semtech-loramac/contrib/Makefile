MODULE := semtech_loramac_contrib

# Use -isystem instead of -I to disable warnings about these headers
INCLUDES += -isystem$(PKGDIRBASE)/semtech-loramac/src/mac \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/boards \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/radio \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/system/crypto \
            -isystem$(PKGDIRBASE)/semtech-loramac/src

include $(RIOTBASE)/Makefile.base
