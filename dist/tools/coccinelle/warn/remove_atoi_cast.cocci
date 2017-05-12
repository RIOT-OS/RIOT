// removes not needed casts like "e = (uint16_t) atoi(e2)"

@@
expression e;
expression e2;
type t;
@@

e =
- (t)
atoi(e2)
