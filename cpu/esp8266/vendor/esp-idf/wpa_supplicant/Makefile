ifneq (,$(filter esp_idf_wpa_supplicant_port ,$(USEMODULE)))
    DIRS += port
endif

ifneq (,$(filter esp_idf_wpa_supplicant_crypto ,$(USEMODULE)))
    DIRS += src/crypto
endif

include $(RIOTBASE)/Makefile.base
