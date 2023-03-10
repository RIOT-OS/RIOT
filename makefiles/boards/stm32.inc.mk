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

ifeq (stm32flash,$(PROGRAMMER))
  ROM_OFFSET ?= 0x0
  FLASHER = stm32flash
  DEBUGGER =
  FLASHFILE ?= $(BINFILE)
  PROG_BAUD ?= 57600
  BIN_ADDR ?= $(shell echo  $$(($(ROM_START_ADDR) + $(ROM_OFFSET))))
  STM32FLASH_RESET ?= 0
  STM32FLASH_RESET_INVERT ?= 1
  # sequence to reset board into bootloader:
  #   BOOT0 --> HIGH, RST --> LOW
  #   BOOT0 --> HIGH, RST --> HIGH
  # sequence to reset board into application
  #   BOOT0 --> LOW, RST --> LOW
  #   BOOT0 --> LOW, RST --> HIGH
  # This can be done via a TTL adapter when BOOT0 is connected to RTS and
  # RST is connected to DTR
  ifeq (1,$(STM32FLASH_RESET))
    # The TTL adapter this was tested with actually has inverted outputs. But
    # we can just support both via a simple flag:
    ifeq (1,$(STM32FLASH_RESET_INVERT))
      FFLAGS += -i '-rts,dtr,-dtr:rts,dtr,-dtr'
      # set term flags so BOOT0 is low and RST is high
      MINITERMFLAGS += --rts 1 --dtr 0
      PYTERMFLAGS += --set-rts 1 --set-dtr 0
    else
      FFLAGS += -i 'rts,-dtr,dtr:-rts,-dtr,dtr'
      # set term flags so BOOT0 is low and RST is high
      MINITERMFLAGS += --rts 0 --dtr 1
      PYTERMFLAGS += --set-rts 0 --set-dtr 1
    endif
  endif
  FFLAGS += -v -b $(PROG_BAUD) -w $(FLASHFILE) -S $(BIN_ADDR) -g $(BIN_ADDR) $(PORT)
endif
