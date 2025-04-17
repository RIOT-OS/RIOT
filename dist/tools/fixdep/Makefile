all: fixdep

fixdep_patched.c: fixdep.c
	patch $^ fixdep_riot.patch -o $@

fixdep: fixdep_patched.c
	$(CC) -O3 -Wall $^ -o $@

clean:
	$(RM) fixdep_patched.c
	$(RM) fixdep

distclean: clean
