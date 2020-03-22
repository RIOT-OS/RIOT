ifneq (,$(filter esp_idf_wpa_supplicant_port,$(USEMODULE)))
    DIRS += port
endif

ifneq (,$(filter esp_idf_wpa_supplicant_crypto,$(USEMODULE)))
    DIRS += src/crypto
endif

ifneq (,$(filter esp_idf_wpa_supplicant_wpa2_eap_peer,$(USEMODULE)))
    DIRS += src/wpa2/eap_peer
endif

ifneq (,$(filter esp_idf_wpa_supplicant_wpa2_tls,$(USEMODULE)))
    DIRS += src/wpa2/tls
endif

ifneq (,$(filter esp_idf_wpa_supplicant_wpa2_utils,$(USEMODULE)))
    DIRS += src/wpa2/utils
endif

include $(RIOTBASE)/Makefile.base
