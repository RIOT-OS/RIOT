# Put defined MCU peripherals here (in alphabetical order)
FEATURES_PROVIDED += periph_dma
FEATURES_PROVIDED += periph_eth
FEATURES_PROVIDED += periph_i2c
FEATURES_PROVIDED += periph_ltdc
FEATURES_PROVIDED += periph_rtc
FEATURES_PROVIDED += periph_rtt
FEATURES_PROVIDED += periph_spi
FEATURES_PROVIDED += periph_timer
FEATURES_PROVIDED += periph_uart
FEATURES_PROVIDED += periph_usbdev

# stm32f746g-disco provides a custom default Kconfig clock configuration
KCONFIG_BOARD_CONFIG += $(RIOTBOARD)/stm32f746g-disco/clock.config
