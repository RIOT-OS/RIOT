// this detects NULL tests that can only be reached when the value is known
// not to be NULL
//
// Confidence: High
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// URL: http://coccinelle.lip6.fr/rules/notnull.html
// Options:

@r exists@
local idexpression x;
expression E;
position p1,p2;
@@

if (x@p1 == NULL || ...) { ... when forall
   return ...; }
... when != \(x=E\|x--\|x++\|--x\|++x\|x-=E\|x+=E\|x|=E\|x&=E\)
    when != &x
(
x@p2 == NULL
|
x@p2 != NULL
)

// another path to the test that is not through p1?

@s exists@
local idexpression r.x;
position r.p1,r.p2;
@@

... when != x@p1
(
x@p2 == NULL
|
x@p2 != NULL
)

// another path to the test from p1?

@t exists@
local idexpression x;
position r.p1,r.p2;
@@

if (x@p1 == NULL || ...) { ... x@p2 ... when any
   return ...; }

// another path to the test containing an assignment?

@u exists@
local idexpression x;
expression E;
position r.p1,r.p2;
@@

if (x@p1 == NULL || ...) { ... when forall
   return ...; }
 ...
 \(x=E\|x--\|x++\|--x\|++x\|x-=E\|x+=E\|x|=E\|x&=E\|&x\)
 ...  when != x@p1
      when any
(
x@p2 == NULL
|
x@p2 != NULL
)

@fix depends on !s && !t && !u@
position r.p2;
expression x,E;
statement S1,S2;
@@

(
- if ((x@p2 != NULL) || ...)
  S1
|
- if ((x@p2 != NULL) || ...)
  S1
- else S2
|
- (x@p2 != NULL) && E
+ E
|
- (x@p2 == NULL) || E
+ E
|
- if ((x@p2 == NULL) && ...) S1
|
- if ((x@p2 == NULL) && ...) S1 else
  S2
|
- BUG_ON(x@p2 == NULL);
)

@script:python depends on !s && !t && !u && !fix@
p1 << r.p1;
p2 << r.p2;
@@

cocci.print_main("",p1)
cocci.print_secs("retest",p2)
