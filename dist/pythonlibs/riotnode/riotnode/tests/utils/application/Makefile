.PHONY: all flash reset term

PIDFILE ?= /tmp/riotnode_test_pid
NODEPID = $(shell cat $(firstword $(wildcard $(PIDFILE)) /dev/null))

NODE_WRAPPER ?= ./node.py
APPLICATION ?= ./echo.py

all:
flash:

reset:
	kill -USR1 $(NODEPID) 2>/dev/null || true

term:
	sh -c 'echo $$$$ > $(PIDFILE); exec $(NODE_WRAPPER) $(APPLICATION)'
