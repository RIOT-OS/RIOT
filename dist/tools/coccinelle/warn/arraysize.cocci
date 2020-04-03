// Use the macro ARRAY_SIZE when possible
//
// Confidence: High
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// URL: http://coccinelle.lip6.fr/rules/array.html
// Options: -I ... -all_includes can give more complete results

// This is a modified version for RIOT-OS.
// At first it was used to help with Pull Request 11865
// Now it helps to warn when it is possible to use the macro
// in new code.

@@
type T;
T[] E;
@@

- (sizeof(E)/sizeof(*E))
+ ARRAY_SIZE(E)

@@
type T;
T[] E;
T E2;
@@

- (sizeof(E)/sizeof(E2))
+ ARRAY_SIZE(E)

@@
type T;
T[] E;
@@

- (sizeof(E)/sizeof(E[...]))
+ ARRAY_SIZE(E)

@@
type T;
T[] E;
@@

- (sizeof(E)/sizeof(T))
+ ARRAY_SIZE(E)

@n@
identifier AS,E;
@@

- #define AS(E) ARRAY_SIZE(E)

@@
expression E;
identifier n.AS;
@@

- AS(E)
+ ARRAY_SIZE(E)
