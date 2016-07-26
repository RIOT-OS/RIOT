# pyjam build file. See https://github.com/kaspar030/pyjam for info.

default.CFLAGS = "-O3 -DUHCP_SYSTEM_LINUX -DUHCP_SERVER"

Main("uhcpd")
