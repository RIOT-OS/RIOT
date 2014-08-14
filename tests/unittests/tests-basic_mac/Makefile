MODULE = tests-basic_mac

RANDOM_8BIT = $(shell od -vAn -N1 -td1 < /dev/urandom | xargs echo)
RANDOM_16BIT = $(shell od -vAn -N2 -td2 < /dev/urandom | xargs echo)
RANDOM_32BIT = $(shell od -vAn -N4 -td4 < /dev/urandom | xargs echo)
RANDOM_64BIT = $(shell od -vAn -N8 -td8 < /dev/urandom | xargs echo)
RANDOM_U8BIT = $(shell od -vAn -N1 -tu1 < /dev/urandom | xargs echo)
RANDOM_U16BIT = $(shell od -vAn -N2 -tu2 < /dev/urandom | xargs echo)
RANDOM_U32BIT = $(shell od -vAn -N4 -tu4 < /dev/urandom | xargs echo)
RANDOM_U64BIT = $(shell od -vAn -N8 -tu8 < /dev/urandom | xargs echo)
RANDOM_STRING = "$(shell head -c "$(1)" /dev/urandom | base64 | head -c "$(1)")"

include $(RIOTBASE)/Makefile.base

CFLAGS += -DRAND_STRING8="\"$(call RANDOM_STRING,8)\""
CFLAGS += -DRAND_STRING12="\"$(call RANDOM_STRING,12)\""
CFLAGS += -DRAND_STRING16="\"$(call RANDOM_STRING,16)\""
CFLAGS += -DRAND_STRING64="\"$(call RANDOM_STRING,64)\""
CFLAGS += -DRAND_INT8=$(RANDOM_8BIT)
CFLAGS += -DRAND_INT16=$(RANDOM_16BIT)
CFLAGS += -DRAND_INT32=$(RANDOM_32BIT)
CFLAGS += -DRAND_INT64=$(RANDOM_64BIT)
CFLAGS += -DRAND_UINT8=$(RANDOM_U8BIT)U
CFLAGS += -DRAND_UINT16=$(RANDOM_U16BIT)U
CFLAGS += -DRAND_UINT32=$(RANDOM_U32BIT)U
CFLAGS += -DRAND_UINT64=$(RANDOM_U64BIT)U
