BOARD ?= rpi-pico-2-arm

# The test pokes RP2350-specific registers and the rp2350_common
# UART driver directly. Limit to boards that use the rp2350_common BSP.
BOARDS_SUPPORTED := rpi-pico-2-arm rpi-pico-2-riscv

# This test stresses the UART RX FIFO under sustained 115200-baud
# traffic; even small debug prints from the interrupt path add ISR
# latency that overruns the FIFO and confounds the result. Opt out
# of DEVELHELP so the RISC-V port's per-trap debug prints (under
# #ifdef DEVELHELP in cpu/riscv_common/irq_arch.c and xh3irq.c) and
# similar diagnostics elsewhere are compiled out. Set here BEFORE
# the include of Makefile.cpu_common so it wins against the
# tests/Makefile.tests_common default of DEVELHELP ?= 1.
DEVELHELP ?= 0

include ../Makefile.cpu_common

USEMODULE += periph_uart

include $(RIOTBASE)/Makefile.include
