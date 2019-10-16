include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon hifive1 hifive1b i-nucleo-lrwan1 nrf6310 \
                             nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f303k8 \
                             nucleo-f334r8 nucleo-l053r8 stm32f0discovery \
                             stm32l0538-disco yunjia-nrf51822

# including lwip_ipv6_mld would currently break this test on at86rf2xx radios
USEMODULE += lwip lwip_ipv6_autoconfig lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += ipv6_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od
USEMODULE += netdev_default

ifeq ($(BOARD),native)
  USEMODULE += lwip_ethernet
endif

# Test only implemented for native
ifneq ($(BOARD),native)
  TESTS=
endif

# test fails on murdock and on my machine due to the process exiting directly
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include
