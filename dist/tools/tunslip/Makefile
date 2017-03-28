PREFIX ?= /usr/local
BIN    = tapslip6 tunslip tunslip6

all: $(BIN)

install:
	mkdir -p $(PREFIX)/bin && install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(foreach bin,$(BIN),$(PREFIX)/bin/$(bin))

clean:
	rm -f $(BIN)

CFLAGS += -Wall -Wextra -pedantic -std=c99
