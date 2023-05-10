include ../Makefile.core_common
FEATURES_REQUIRED += cpp
FEATURES_REQUIRED += libstdcpp
USEPKG += fff

# Some boards don't define irq functions as static inline. Then they can't be mocked.
FEATURES_BLACKLIST += \
    arch_esp32 \
    arch_esp8266 \
    arch_mips32r2 \
    arch_native

include $(RIOTBASE)/Makefile.include
