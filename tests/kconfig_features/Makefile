include ../Makefile.tests_common

BOARD_WHITELIST += 6lowpan-clicker \
                   acd52832 \
                   adafruit-clue \
                   airfy-beacon \
                   arduino-due \
                   arduino-duemilanove \
                   arduino-leonardo \
                   arduino-mega2560 \
                   arduino-mkr1000 \
                   arduino-mkrfox1200 \
                   arduino-mkrwan1300 \
                   arduino-mkrzero \
                   arduino-nano \
                   arduino-nano-33-ble \
                   arduino-uno \
                   arduino-zero \
                   atmega1284p \
                   atmega256rfr2-xpro \
                   atmega328p \
                   avr-rss2 \
                   avsextrem \
                   b-l072z-lrwan1 \
                   b-l475e-iot01a \
                   blackpill \
                   blackpill-128kib \
                   bluepill \
                   bluepill-128kib \
                   calliope-mini \
                   cc1312-launchpad \
                   cc1352-launchpad \
                   cc1352p-launchpad \
                   cc2538dk \
                   cc2650-launchpad \
                   cc2650stk \
                   chronos \
                   derfmega128 \
                   derfmega256 \
                   dwm1001 \
                   ek-lm4f120xl \
                   esp32-heltec-lora32-v2 \
                   esp32-mh-et-live-minikit \
                   esp32-olimex-evb \
                   esp32-ttgo-t-beam \
                   esp32-wemos-lolin-d32-pro \
                   esp32-wroom-32 \
                   esp32-wrover-kit \
                   esp8266-esp-12x \
                   esp8266-olimex-mod \
                   esp8266-sparkfun-thing \
                   f4vi1 \
                   feather-m0 \
                   feather-nrf52840 \
                   firefly \
                   fox \
                   frdm-k22f \
                   frdm-k64f \
                   hamilton \
                   hifive1 \
                   hifive1b \
                   ikea-tradfri \
                   im880b \
                   i-nucleo-lrwan1 \
                   iotlab-a8-m3 \
                   iotlab-m3 \
                   limifrog-v1 \
                   lobaro-lorabox \
                   lsn50 \
                   maple-mini \
                   mbed_lpc1768 \
                   mcb2388 \
                   mega-xplained \
                   microbit \
                   microduino-corerf \
                   msb-430 \
                   msb-430h \
                   msba2 \
                   msbiot \
                   mulle \
                   native \
                   nrf51dk \
                   nrf51dongle \
                   nrf52832-mdk \
                   nrf52840-mdk \
                   nrf52840dk \
                   nrf52840dongle \
                   nrf52dk \
                   nrf6310 \
                   nucleo-f030r8 \
                   nucleo-f031k6 \
                   nucleo-f042k6 \
                   nucleo-f070rb \
                   nucleo-f072rb \
                   nucleo-f091rc \
                   nucleo-f103rb \
                   nucleo-f207zg \
                   nucleo-f302r8 \
                   nucleo-f303k8 \
                   nucleo-f303re \
                   nucleo-f303ze \
                   nucleo-f334r8 \
                   nucleo-f401re \
                   nucleo-f410rb \
                   nucleo-f411re \
                   nucleo-f412zg \
                   nucleo-f413zh \
                   nucleo-f429zi \
                   nucleo-f446re \
                   nucleo-f446ze \
                   nucleo-f722ze \
                   nucleo-f746zg \
                   nucleo-f767zi \
                   nucleo-g474re \
                   nucleo-l031k6 \
                   nucleo-l053r8 \
                   nucleo-l073rz \
                   nucleo-l152re \
                   nucleo-l412kb \
                   nucleo-l432kc \
                   nucleo-l433rc \
                   nucleo-l452re \
                   nucleo-l476rg \
                   nucleo-l496zg \
                   nucleo-l4r5zi \
                   nz32-sc151 \
                   olimexino-stm32 \
                   opencm904 \
                   openlabs-kw41z-mini \
                   openlabs-kw41z-mini-256kib \
                   openmote-b \
                   openmote-cc2538 \
                   p-l496g-cell02 \
                   p-nucleo-wb55 \
                   particle-argon \
                   particle-boron \
                   particle-xenon \
                   pba-d-01-kw2x \
                   phynode-kw41z \
                   pic32-wifire \
                   pinetime \
                   pyboard \
                   reel \
                   remote-pa \
                   remote-reva \
                   remote-revb \
                   ruuvitag \
                   samd21-xpro \
                   same54-xpro \
                   saml10-xpro \
                   saml11-xpro \
                   saml21-xpro \
                   samr21-xpro \
                   samr30-xpro \
                   samr34-xpro \
                   seeeduino_arch-pro \
                   sensebox_samd21 \
                   serpente \
                   slstk3401a \
                   slstk3402a \
                   sltb001a \
                   slwstk6000b-slwrb4150a \
                   slwstk6000b-slwrb4162a \
                   slwstk6220a \
                   spark-core \
                   stk3600 \
                   stk3700 \
                   stm32f030f4-demo \
                   stm32f0discovery \
                   stm32f3discovery \
                   stm32f4discovery \
                   stm32f429i-disc1 \
                   stm32f429i-disco \
                   stm32f723e-disco \
                   stm32f769i-disco \
                   stm32l0538-disco \
                   stm32l476g-disco \
                   teensy31 \
                   telosb \
                   thingy52 \
                   ublox-c030-u201 \
                   udoo \
                   usb-kw41z \
                   waspmote-pro \
                   weact-f411ce \
                   yunjia-nrf51822 \
                   z1
                   #

all: checks

include $(RIOTBASE)/Makefile.include

# List of variables to compare between Makefile and Kconfig
_VARS_TO_CHECK = BOARD CPU CPU_MODEL CPU_FAM CPU_CORE CPU_ARCH

# Commands to diff in 'check-values'
_CMDS = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-$(v)))
_CMDS_CONFIG = $(foreach v,$(_VARS_TO_CHECK), $(if $($(v)),info-debug-variable-CONFIG_$(v)))

# Only compare variables that have a value in Makefile
checks: kconfig-features check-values

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

check-values: $(KCONFIG_OUT_CONFIG)
	@bash -c '(diff <($(MAKE) $(_CMDS) | sort) \
	    <($(MAKE) $(_CMDS_CONFIG) | sort) && echo "SUCCESS: $(_CMDS:info-debug-variable-%=%) values match") || \
	    (echo "ERROR: The value for $(_CMDS:info-debug-variable-%=%) in Kconfig does not match the one in the Makefile" && exit 1)'
