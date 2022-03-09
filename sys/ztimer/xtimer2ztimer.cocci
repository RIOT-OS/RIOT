// coccinelle script for converting xtimer usage to ztimer API
// this adds the ZTIMER_USEC as clock parameter, unless a multiplication of
// known constants (US_PER_MS, ...) makes it feasable to us a slower clock.

@@
@@

- #include "xtimer.h"
+ #include "ztimer.h"

@@
expression e;
@@

- xtimer_usleep(e)
+ ztimer_sleep(ZTIMER_USEC, e)

@@
expression e;
@@

- xtimer_msleep(e)
+ ztimer_sleep(ZTIMER_MSEC, e)

@@
expression e;
@@

- xtimer_sleep(e)
+ ztimer_sleep(ZTIMER_MSEC, e * 1000)

@@
expression e;
@@

- xtimer_set(e)
+ ztimer_set(ZTIMER_USEC, e)

@@
expression e;
@@

- xtimer_remove(e)
+ ztimer_remove(ZTIMER_USEC, e)

@@
expression e;
@@

- xtimer_now_usec(e)
+ ztimer_now(ZTIMER_USEC, e)

@@
expression e;
@@

- xtimer_now_msec(e)
+ ztimer_now(ZTIMER_MSEC, e)

@@
expression e, e2;
@@

- xtimer_set(e, e2)
+ ztimer_set(ZTIMER_USEC, e, e2)

@@
identifier i;
@@

- xtimer_t i
+ ztimer_t i
  = ...;

@@
identifier i;
@@

- xtimer_t i;
+ ztimer_t i;

@@
@@

- xtimer_now_usec()
+ ztimer_now(ZTIMER_USEC)

@@
@@

- _xtimer_now()
+ ztimer_now(ZTIMER_USEC)

@@
@@

- xtimer_now_usec64()
+ ztimer_now64()

@@
identifier i;
@@

struct {
...
- xtimer_t i;
+ ztimer_t i;
...
}

@@
identifier i, i2;
@@

struct i2 {
...
- xtimer_t i;
+ ztimer_t i;
...
}


@@
identifier fn;
identifier i;
@@

  fn(...,
-    xtimer_t i
+    ztimer_t i
     ,...) {...}

@@
identifier fn;
identifier i;
@@

  fn(...,
-    xtimer_t *i
+    ztimer_t *i
     ,...) {...}

@@
expression e1, e2, e3, e4;
@@

- xtimer_set_msg(e1,e2,e3,e4)
+ ztimer_set_msg(ZTIMER_USEC, e1, e2, e3, e4)

@@
expression e;
@@

- xtimer_usleep64(e * US_PER_MS)
+ ztimer_sleep(ZTIMER_MSEC, e)

@@
expression e, e2;
@@

- xtimer_msg_receive_timeout(e, e2 * US_PER_MS)
+ ztimer_msg_receive_timeout(ZTIMER_MSEC, e, e2)

@@
expression e, e2;
@@

- xtimer_msg_receive_timeout(e, e2)
+ ztimer_msg_receive_timeout(ZTIMER_USEC, e, e2)

@@
identifier i;
expression e;
@@

- xtimer_ticks32_t i = e;
+ uint32_t i = e;

@@
identifier i;
@@

- xtimer_ticks32_t i;
+ uint32_t i;

@@
expression e;
@@

- xtimer_usec_from_ticks(e)
+ e

@@
@@

- xtimer_now()
+ ztimer_now(ZTIMER_USEC)

@@
@@

- MODULE_XTIMER
+ MODULE_ZTIMER

@@
expression e, e2;
@@

- xtimer_periodic_wakeup(e, e2)
+ ztimer_periodic_wakeup(ZTIMER_USEC, e, e2)

@@
expression e, e2;
@@

- xtimer_set_timeout_flag(e, e2)
+ ztimer_set_timeout_flag(ZTIMER_USEC, e, e2)


@@
expression e, e2;
@@

- xtimer_mutex_lock_timeout(e, e2)
+ ztimer_mutex_lock_timeout(ZTIMER_USEC, e, e2)

@@
expression e, e2;
@@

- xtimer_msg_receive_timeout(e, e2)
+ ztimer_msg_receive_timeout(ZTIMER_USEC, e, e2)
