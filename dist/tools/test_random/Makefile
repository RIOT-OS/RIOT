RIOTBASE := ../../..
INCLUDES := $(RIOTBASE)/sys/include

CFLAGS += $(patsubst %,-I%,$(INCLUDES))
CFLAGS += -Wall -O3

PRNGS := mersenne minstd musl_lcg

comma:= ,
empty:=
space:= $(empty) $(empty)
PRNGS_CSV:= $(subst $(space),$(comma),$(PRNGS))

all: $(PRNGS:%=test_%) $(PRNGS:%=test_%_entropy)

clean:
	rm -f test_*_entropy
	rm -f test_{$(PRNGS_CSV)}

test_%: test_random.c $(RIOTBASE)/sys/random/%.c
	$(CC) $(CFLAGS) $^ -o $@$(SUFFIX)

test_%_entropy: test_random.c $(RIOTBASE)/sys/random/%.c $(RIOTBASE)/sys/random/entropy.c
	$(CC) $(CFLAGS) -DMODULE_ENTROPY -DURANDOM $^ -o $@$(SUFFIX)

run_tests: all
	for prng in $(PRNGS) ; do \
		./test_$${prng} > test_$${prng}_out ; \
		ent test_$${prng}_out > test_$${prng}.ent.txt ; \
		for freq in 1024 4096 16384 65536 262144 1048576 4194304 16777216; do \
			./test_$${prng}_entropy $${freq}> test_$${prng}_entropy_freq_$${freq}_out ; \
			ent test_$${prng}_entropy_freq_$${freq}_out > test_$${prng}_entropy_freq_$${freq}.ent.txt; \
			done ; \
		done
