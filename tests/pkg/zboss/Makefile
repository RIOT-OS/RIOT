include ../Makefile.pkg_common

USEPKG += zboss

# some boards are missing FLASHPAGE_SIZE
# TODO: use flashpage correctly inside ZBOSS
BOARD_BLACKLIST = \
  msbiot \
  avsextrem \
  esp32-ethernet-kit-v1_0 \
  esp32-ethernet-kit-v1_1 \
  esp32-ethernet-kit-v1_2 \
  esp32-heltec-lora32-v2 \
  esp32-mh-et-live-minikit \
  esp32-olimex-evb \
  esp32-ttgo-t-beam \
  esp32-wemos-lolin-d32-pro \
  esp32-wroom-32 \
  esp32-wrover-kit \
  esp32s2-devkit \
  esp32s2-lilygo-ttgo-t8 \
  esp32s2-wemos-mini \
  esp32s3-box \
  esp32s3-devkit \
  esp32s3-pros3 \
  esp32s3-usb-otg \
  esp32s3-wt32-sc01-plus\
  f4vi1\
  mcb2388 \
  msba2 \
  nucleo-f070rb \
  nucleo-f207zg \
  nucleo-f401re \
  nucleo-f410rb \
  nucleo-f411re \
  nucleo-f412zg \
  nucleo-f413zh \
  nucleo-f429zi \
  nucleo-f439zi \
  nucleo-f446re \
  nucleo-f446ze \
  nucleo-f722ze \
  nucleo-f746zg \
  nucleo-f767zi \
  nucleo-l011k4 \
  nucleo-l031k6 \
  pyboard \
  samd10-xmini \
  same51-curiosity-nano \
  stm32f030f4-demo \
  stm32f429i-disc1 \
  stm32f429i-disco \
  stm32f469i-disco \
  stm32f4discovery \
  stm32f723e-disco \
  stm32f746g-disco \
  stm32f7508-dk \
  stm32f769i-disco \
  ublox-c030-u201 \
  weact-f401cc \
  weact-f401ce \
  weact-f411ce

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += ztimer_sec

include $(RIOTBASE)/Makefile.include
