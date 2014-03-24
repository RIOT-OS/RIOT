CC = gcc
CFLAGS = -O
INCLUDE =-I./tbcuppa/
RM = rm

all: bcuppa tcuppa buma tuma

bcuppa:./tbcuppa/bcuppa.c ./tbcuppa/strvec.c
	$(CC) -o $@ $(INCLUDE) ./tbcuppa/bcuppa.c ./tbcuppa/strvec.c

tcuppa:./tbcuppa/tcuppa.c ./tbcuppa/strvec.c
	$(CC) -o $@ $(INCLUDE) ./tbcuppa/tcuppa.c ./tbcuppa/strvec.c

buma:./tbuma/buma.c ./tbcuppa/strvec.c
	$(CC) -o $@ $(INCLUDE) ./tbuma/buma.c ./tbcuppa/strvec.c
	
tuma:./tbuma/tuma.c ./tbcuppa/strvec.c
	$(CC) -o $@ $(INCLUDE) ./tbuma/tuma.c ./tbcuppa/strvec.c

clean:
	$(RM) bcuppa tcuppa buma tuma

.PHONY: clean all
