MODULE = esp_idf_wpa_supplicant

# source file list to be compiled as configured in component.mk
ESP32_SDK_SRC = \
  components/wpa_supplicant/esp_supplicant/src/esp_dpp.c \
  components/wpa_supplicant/esp_supplicant/src/esp_hostap.c \
  components/wpa_supplicant/esp_supplicant/src/esp_wpa2.c \
  components/wpa_supplicant/esp_supplicant/src/esp_wpa3.c \
  components/wpa_supplicant/esp_supplicant/src/esp_wpa_main.c \
  components/wpa_supplicant/esp_supplicant/src/esp_wpas_glue.c \
  components/wpa_supplicant/esp_supplicant/src/esp_wps.c \
  components/wpa_supplicant/port/os_xtensa.c \
  components/wpa_supplicant/src/ap/ap_config.c \
  components/wpa_supplicant/src/ap/ieee802_1x.c \
  components/wpa_supplicant/src/ap/wpa_auth.c \
  components/wpa_supplicant/src/ap/wpa_auth_ie.c \
  components/wpa_supplicant/src/common/dpp.c \
  components/wpa_supplicant/src/common/sae.c \
  components/wpa_supplicant/src/common/wpa_common.c \
  components/wpa_supplicant/src/crypto/aes-cbc.c \
  components/wpa_supplicant/src/crypto/aes-ccm.c \
  components/wpa_supplicant/src/crypto/aes-ctr.c \
  components/wpa_supplicant/src/crypto/aes-gcm.c \
  components/wpa_supplicant/src/crypto/aes-internal.c \
  components/wpa_supplicant/src/crypto/aes-internal-dec.c \
  components/wpa_supplicant/src/crypto/aes-internal-enc.c \
  components/wpa_supplicant/src/crypto/aes-omac1.c \
  components/wpa_supplicant/src/crypto/aes-siv.c \
  components/wpa_supplicant/src/crypto/aes-unwrap.c \
  components/wpa_supplicant/src/crypto/aes-wrap.c \
  components/wpa_supplicant/src/crypto/ccmp.c \
  components/wpa_supplicant/src/crypto/crypto_internal.c \
  components/wpa_supplicant/src/crypto/crypto_internal-cipher.c \
  components/wpa_supplicant/src/crypto/crypto_internal-modexp.c \
  components/wpa_supplicant/src/crypto/crypto_internal-rsa.c \
  components/wpa_supplicant/src/crypto/crypto_ops.c \
  components/wpa_supplicant/src/crypto/des-internal.c \
  components/wpa_supplicant/src/crypto/dh_group5.c \
  components/wpa_supplicant/src/crypto/dh_groups.c \
  components/wpa_supplicant/src/crypto/md4-internal.c \
  components/wpa_supplicant/src/crypto/md5.c \
  components/wpa_supplicant/src/crypto/md5-internal.c \
  components/wpa_supplicant/src/crypto/ms_funcs.c \
  components/wpa_supplicant/src/crypto/rc4.c \
  components/wpa_supplicant/src/crypto/sha1.c \
  components/wpa_supplicant/src/crypto/sha1-internal.c \
  components/wpa_supplicant/src/crypto/sha1-pbkdf2.c \
  components/wpa_supplicant/src/crypto/sha1-prf.c \
  components/wpa_supplicant/src/crypto/sha1-tlsprf.c \
  components/wpa_supplicant/src/crypto/sha256.c \
  components/wpa_supplicant/src/crypto/sha256-internal.c \
  components/wpa_supplicant/src/crypto/sha256-kdf.c \
  components/wpa_supplicant/src/crypto/sha256-prf.c \
  components/wpa_supplicant/src/crypto/sha256-tlsprf.c \
  components/wpa_supplicant/src/crypto/sha384-internal.c \
  components/wpa_supplicant/src/crypto/sha384-prf.c \
  components/wpa_supplicant/src/crypto/sha384-tlsprf.c \
  components/wpa_supplicant/src/crypto/sha512-internal.c \
  components/wpa_supplicant/src/eap_peer/chap.c \
  components/wpa_supplicant/src/eap_peer/eap.c \
  components/wpa_supplicant/src/eap_peer/eap_common.c \
  components/wpa_supplicant/src/eap_peer/eap_mschapv2.c \
  components/wpa_supplicant/src/eap_peer/eap_peap.c \
  components/wpa_supplicant/src/eap_peer/eap_peap_common.c \
  components/wpa_supplicant/src/eap_peer/eap_tls.c \
  components/wpa_supplicant/src/eap_peer/eap_tls_common.c \
  components/wpa_supplicant/src/eap_peer/eap_ttls.c \
  components/wpa_supplicant/src/eap_peer/mschapv2.c \
  components/wpa_supplicant/src/rsn_supp/pmksa_cache.c \
  components/wpa_supplicant/src/rsn_supp/wpa.c \
  components/wpa_supplicant/src/rsn_supp/wpa_ie.c \
  components/wpa_supplicant/src/tls/asn1.c \
  components/wpa_supplicant/src/tls/bignum.c \
  components/wpa_supplicant/src/tls/pkcs1.c \
  components/wpa_supplicant/src/tls/pkcs5.c \
  components/wpa_supplicant/src/tls/pkcs8.c \
  components/wpa_supplicant/src/tls/rsa.c \
  components/wpa_supplicant/src/tls/tls_internal.c \
  components/wpa_supplicant/src/tls/tlsv1_client.c \
  components/wpa_supplicant/src/tls/tlsv1_client_read.c \
  components/wpa_supplicant/src/tls/tlsv1_client_write.c \
  components/wpa_supplicant/src/tls/tlsv1_common.c \
  components/wpa_supplicant/src/tls/tlsv1_cred.c \
  components/wpa_supplicant/src/tls/tlsv1_record.c \
  components/wpa_supplicant/src/tls/tlsv1_server.c \
  components/wpa_supplicant/src/tls/tlsv1_server_read.c \
  components/wpa_supplicant/src/tls/tlsv1_server_write.c \
  components/wpa_supplicant/src/tls/x509v3.c \
  components/wpa_supplicant/src/utils/base64.c \
  components/wpa_supplicant/src/utils/bitfield.c \
  components/wpa_supplicant/src/utils/common.c \
  components/wpa_supplicant/src/utils/ext_password.c \
  components/wpa_supplicant/src/utils/json.c \
  components/wpa_supplicant/src/utils/uuid.c \
  components/wpa_supplicant/src/utils/wpabuf.c \
  components/wpa_supplicant/src/utils/wpa_debug.c \
  components/wpa_supplicant/src/wps/wps_attr_build.c \
  components/wpa_supplicant/src/wps/wps_attr_parse.c \
  components/wpa_supplicant/src/wps/wps_attr_process.c \
  components/wpa_supplicant/src/wps/wps.c \
  components/wpa_supplicant/src/wps/wps_common.c \
  components/wpa_supplicant/src/wps/wps_dev_attr.c \
  components/wpa_supplicant/src/wps/wps_enrollee.c \
  components/wpa_supplicant/src/wps/wps_registrar.c \
  components/wpa_supplicant/src/wps/wps_validate.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src/utils

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

# definitions for wpa_supplicant from components/wpa_supplicant/component.mk
CFLAGS += -D__ets__
CFLAGS += -DCONFIG_DPP
CFLAGS += -DCONFIG_ECC
CFLAGS += -DCONFIG_IEEE80211W
CFLAGS += -DCONFIG_SHA256
CFLAGS += -DCONFIG_WNM
CFLAGS += -DCONFIG_WPS_PIN
CFLAGS += -DCONFIG_WPS2
CFLAGS += -DEAP_MSCHAPv2
CFLAGS += -DEAP_PEAP
CFLAGS += -DEAP_PEER_METHOD
CFLAGS += -DEAP_TLS
CFLAGS += -DEAP_TTLS
CFLAGS += -DESP_SUPPLICANT
CFLAGS += -DESP32_WORKAROUND
CFLAGS += -DESPRESSIF_USE
CFLAGS += -DIEEE8021X_EAPOL
CFLAGS += -DUSE_WPA2_TASK
CFLAGS += -DUSE_WPS_TASK
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-format-nonliteral
CFLAGS += -Wno-format-security
CFLAGS += -std=gnu99

include ../esp_idf.mk
include ../esp_idf_cflags.mk
