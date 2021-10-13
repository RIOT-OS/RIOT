// ENABLE_DEBUG must not use parenthesis to be compatible with IS_ACTIVE

@@
constant C;
@@

- #define ENABLE_DEBUG (C)
+ #define ENABLE_DEBUG C

// make sure that ENABLE_DEBUG is zero

@@
@@

- #define ENABLE_DEBUG 1
+ #define ENABLE_DEBUG 0
