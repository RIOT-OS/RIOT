PROGRAMMER ?= openocd
PROGRAMMERS_SUPPORTED += bmp dfu-util openocd stm32flash

ifeq (bmp,$(PROGRAMMER))
  # On Blackmagic Probe, the first ACM is used to connect to the gdb server,
  # the second is the BMP's UART interface
  PORT_LINUX ?= /dev/ttyACM1
  PORT_DARWIN ?= $(wordlist 2, 2, $(sort $(wildcard /dev/tty.usbmodem*)))
endif

# STM32 boards can become un-flashable after a hardfault,
# use connect_assert_srst to always be able to flash or reset the boards.
OPENOCD_RESET_USE_CONNECT_ASSERT_SRST ?= 1

# For STM32 boards the ST-link adapter is the default adapter, e.g. all
# Nucleo boards have an on-board ST-link adapter
OPENOCD_DEBUG_ADAPTER ?= stlink

JLINK_DEVICE ?= $(CPU_MODEL)

ifeq (dfu-util,$(PROGRAMMER))
  # optionally, use dfu-util to flash via usb
  # note: needs a bootloader flashed before, config below is compatible
  # with blackmagic_dfu, see https://github.com/blacksphere/blackmagic/
  # To stop bootloader from loading an existing firmware, pull down
  # (ground) GPIO B1.
  ifeq (,$(DFU_USB_ID))
    $(error DFU_USB_ID is not set)
  endif
endif

ifeq (stm32flash,$(PROGRAMMER))
	ROM_OFFSET ?= 0x0
	FLASHER = stm32flash
	DEBUGGER =
	FLASHFILE ?= $(BINFILE)
	PROG_BAUD ?= 57600
	BIN_ADDR ?= $(shell echo  $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
	FFLAGS = -v -b $(PROG_BAUD) -w $(FLASHFILE) -S $(BIN_ADDR) -g $(BIN_ADDR) $(PORT)
endif
