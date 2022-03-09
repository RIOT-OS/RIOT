# Atmel SAM C/D/L/R series:
ifneq (,$(findstring samd09,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd09
else ifneq (,$(findstring samd10,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd10
else ifneq (,$(findstring samd11,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd11
else ifneq (,$(findstring samd20,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd20
else ifneq (,$(findstring samd21,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd21
else ifneq (,$(findstring samc21,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samc21
else ifneq (,$(findstring saml21,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = saml21
else ifneq (,$(findstring saml22,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = saml22
else ifneq (,$(findstring samr21,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samr21
else ifneq (,$(findstring samr30,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samr30
else ifneq (,$(findstring samr34,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samr34
else ifneq (,$(findstring samr35,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samr35
else ifneq (,$(findstring samda1,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samda1
endif

# Atmel SAM3X/A/U series:
ifneq (,$(findstring samg51,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samg51
else ifneq (,$(findstring samg53,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samg53
else ifneq (,$(findstring samg54,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samg54
else ifneq (,$(findstring samg55,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samg55
else ifneq (,$(findstring sam4sd,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sam4sd
else ifneq (,$(findstring sam4sa,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sam4sa
else ifneq (,$(findstring sam4s,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sam4s
else ifneq (,$(findstring sam4e,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sam4e
else ifneq (,$(findstring sam4n,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sam4n
endif

# Atmel SAM E7x/S7x/V7x series:
ifneq (,$(findstring same70,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = same70
else ifneq (,$(findstring sams70,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = sams70
else ifneq (,$(findstring samv71,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samv71
else ifneq (,$(findstring samv70,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samv70
endif

# Atmel SAM D5x/E5x:
ifneq (,$(findstring same51,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = same51
else ifneq (,$(findstring samd51,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = samd51
else ifneq (,$(findstring same53,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = same53
else ifneq (,$(findstring same54,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = same54
endif

# Microchip SAM L10/L11:
ifneq (,$(findstring saml10,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = saml10
else ifneq (,$(findstring saml11,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = saml11
endif

# STMicroelectronics STM32G0 series:
ifneq (,$(findstring stm32g0,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = stm32g0
endif

# GigaDevice GD32F4xx series:
ifneq (,$(findstring gd32f4,$(CPU_MODEL)))
  EDBG_DEVICE_TYPE = gd32f4xx
endif

ifeq (,$(EDBG_DEVICE_TYPE))
  $(error "Device not supported by edbg.")
endif
