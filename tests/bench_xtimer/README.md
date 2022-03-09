# Introduction

This test executes some benchmarks for xtimer's set() / remove() / now()
operations.

# Details

This set of benchmarks measures xtimer's list operation efficiency.
Depending on the available memory, the individual benchmarks that are using
multiple timers are run with either 1000 (the default), 100 or 20 timers.
Each benchmark is repeated REPEAT times (default 1000).
As only the operations are benchmarked, it is asserted that no timer ever
actually triggers.

### set() one

This repeatedly sets one timer in an otherwise empty list.
All but the first iteration will cause xtimer to implicitly remove the timer
first.
All iterations will cause the underlying periph timer to be updated.

### remove() one

This repeatedly removes one timer from the list. In all but the first iteration,
this list will be empty.


### set() + remove() one

This repeatedly first sets, then removes one timer. The list is empty
before and after each iteration.
All iterations will cause the underlying periph timer to be updated.

### set() many increasing targets

This adds NUMOF timers with increasing target times. Each iteration will add a
timer at the end of xtimer's timer list.
Only the first iteration will cause the underlying periph timer to be updated.
After this test, the list is populated with NUMOF timers.

### re-set() first

This repatedly re-sets the first timer in the list (to the same target time).
All iterations will cause the underlying periph timer to be updated.

### re-set() middle

This repatedly re-sets the timer in the middle of the list (to the same target
time).

### re-set() last

This repatedly re-sets the last timer in the list (to the same target time).

### remove() + set() first, middle, last

Same as the previous three, but does a remove() before set().

### remove() many decreasing

This removes all timers from the list, starting with the last.

### xtimer_now()

This simply calls xtimer_now() in a loop.


# How to interpret results

The aim is to measure the time spent in xtimer's list operations.
Lower values are better.
The first/middle/last tests give an idea of the best case / average case /
worst case when running the operation with NUMOF timers.
Note that every set() on an already set timer will trigger an implicit remove(),
thus the timer list has to be iterated twice.
The tests that do a remove() before set() show whether xtimer correctly
identifies an unset timer.
