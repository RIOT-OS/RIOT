PKG_NAME    = qcbor
PKG_URL     = https://github.com/laurencelundblade/QCBOR
# v1.6
PKG_VERSION = 590a23daf65af068cee81555c597063150e23539
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-type-limits

# - newlib for some ESP variants does not implement the `feclearexcept` and
#   `fetestexcept` functions from `fenv.h`.
# - picolibc in general and newlib for RISC-V only define the floating-point
#   exception macros (`FE_INVALID`, ...) used in `fenv.h` if the target has an FPU.
# - newlib for many other architectures *does* always define these functions or
#   macros, but it's better not to rely on that.
# -> Disable the HW FPU support to work around these issues.
ifneq (,$(filter esp8266 esp32s2, $(CPU_FAM)))
  CFLAGS += -DQCBOR_DISABLE_FLOAT_HW_USE
else ifneq (,$(filter rv32 rv64,$(CPU_ARCH)))
  # Currently there is no `riscv_fpu` module that would tell us whether or not
  # a HW FPU is present, so it has to be disabled for all RISC-Vs.
  CFLAGS += -DQCBOR_DISABLE_FLOAT_HW_USE
else ifeq (,$(filter cortexm_fpu,$(USEMODULE)))
  CFLAGS += -DQCBOR_DISABLE_FLOAT_HW_USE
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
