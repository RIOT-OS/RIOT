// A pointer should not be compared to zero
//
// Confidence: High
// Copyright: (C) Gilles Muller, Julia Lawall, EMN, INRIA, DIKU.  GPLv2.
// URL: http://coccinelle.lip6.fr/rules/badzero.html
// Options:

@ disable is_zero,isnt_zero @
expression *E;
expression E1,f;
@@

E = f(...)
<...
(
- E == 0
+ !E
|
- E != 0
+ E
|
- 0 == E
+ !E
|
- 0 != E
+ E
)
...>
?E = E1

@ disable is_zero,isnt_zero @
expression *E;
@@

(
  E ==
- 0
+ NULL
|
  E !=
- 0
+ NULL
|
- 0
+ NULL
  == E
|
- 0
+ NULL
  != E
)
