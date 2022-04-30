MODULE := rest_client_transport
SUBMODULES := 1
BASE_MODULE := rest_client_transport

SRC += util.c

ifneq (,$(filter rest_client_transport_mqtt%,$(USEMODULE)))
  SRC += mqttnet.c
  SRC += mqttexample.c
endif

include $(RIOTBASE)/Makefile.base
