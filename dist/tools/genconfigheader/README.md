genconfigheader
===============

Usage: `env CFLAGS="..." genconfigheader path/to/header.h path/to/cflags.mk`

This tool:

1. Takes the `CFLAGS` from the environment and hashes it
2. Read the given header (if existing) and checks if the hash stored in it
   matches. If so, it exits early without touching the header and makefile
3. De-duplicates any `-D...` `-U...` macro flags that match perfectly
4. Fails on any conflicting macro flags, e.g. `-DFOO=1` and `-DFOO=2`,
   `-DFOO` and `-DFOO=1`, or `-DFOO` and `-UFOO`
5. Writes `CFLAGS := <$CFLAGS without -D... and -U>` to the given makefile
6. Writes equivalent `#define ...` and `#undef ...` directives to the given
   header and adds the hash of the `CFLAGS` to it (as magic comment)

The goal of this script is to:

1. Cause changes in CFLAGS for to (indirectly) cause rebuilds
2. Deduplicate macro definitions
3. Detect conflicting macro definitions early
