MODULE = wakaama_client

SRC =                       \
    object_server.c         \
    object_security.c       \
    object_access_control.c \
    #

include $(RIOTBASE)/Makefile.base
