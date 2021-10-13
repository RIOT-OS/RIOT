PKG_NAME=stm32cmsis

# The package URL depends on the family of the target STM32
PKG_URL=https://github.com/STMicroelectronics/cmsis_device_$(CPU_FAM)

PKG_VERSION_f0=70d2069248b18e1cce13a035abc3230abe48754f  # v2.3.4
PKG_VERSION_f1=2948138428461c0621fd53b269862c6e6bb043ce  # v4.3.1
PKG_VERSION_f2=2415e69642f0d0afd256ec93fa4ba1ac010b348e  # v2.2.3
PKG_VERSION_f3=aa2333a5313f65eb1a63bd876edb455197b38f91  # v2.3.4
PKG_VERSION_f4=4cf505380bf123941fd485b1ae01c0747dca3792  # v2.6.4
PKG_VERSION_f7=71be130bda6963d9d8ddbb71530477eadf0be740  # v1.2.5
PKG_VERSION_g0=92ba98253dd6a748a04be14d3e436be3a35d6880  # v1.3.0
PKG_VERSION_g4=7194c403cd5e4bd8bd4621a21d366d525e61b998  # v1.1.1
PKG_VERSION_l0=78696f4f79cf51c8b32d0aa5c199a04f57ee4b27  # v1.9.0
PKG_VERSION_l1=4a2f3cd6ea4bbc8e36daff0c7574b0fc1dfb7df7  # v2.3.0
PKG_VERSION_l4=e442c72651e8d4757f6562acc14da949644944ce  # v1.6.1
PKG_VERSION_l5=d922865fc0326a102c26211c44b8e42f52c1e53d  # v1.0.3
PKG_VERSION_wb=44490652c88a513ec9a892c328db0246cf113987  # v1.4.0
PKG_VERSION_wl=35631ff4d3a3cc361241af5f62190d0ea43bd65b  # v1.0.0

PKG_VERSION=$(PKG_VERSION_$(CPU_FAM))
PKG_LICENSE=Apache-2

# Store the cloned repositories under the stm32 directory to prevent downloads
# for each builds.
PKG_SOURCE_DIR = $(RIOTBASE)/cpu/stm32/include/vendor/cmsis/$(CPU_FAM)
PKG_PATCH_DIR = $(CURDIR)/patches/$(CPU_FAM)

include $(RIOTBASE)/pkg/pkg.mk

all:
	@:
