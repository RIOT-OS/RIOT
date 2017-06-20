// remove unused static property of function-local static variable
// (from Julia Lawall, see https://lists.riot-os.org/pipermail/devel/2017-May/005133.html)

@bad exists@
position p;
identifier x;
type T;
@@

static T x@p;
...
x = <+...x...+>

@@
identifier x;
expression e;
type T;
position p != bad.p;
@@

-static
 T x@p;
 ... when != x
     when strict
?x = e;
