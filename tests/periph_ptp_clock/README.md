Peripheral PTP Clock Test Application
=====================================

Hardware Requirements
---------------------

In addition to the `periph_ptp` feature, this tests requires the peripheral
timer to be run on a clock synchronous to the PTP clock. Otherwise clock
drift between the two clocks might result in the test failing even though the
driver works correctly.

Verifying Clock Speed Adjustment
--------------------------------

In the first part of the test the speed of the PTP clock is adjusted to
different values. The test sleeps for 0.5 seconds and verifies that the clock
drift between the high level timer API and PTP clock matches the speed
adjustment (± 0.01%).

Verifying Clock Adjustment
--------------------------

The second part of the test verifies that adjusting the clock does not
introduce noticeable errors. The clock speed is set back to nominal speed
prior to this test part (so that it synchronous to the high level timer). For
each of a set of predefined clock adjustment values the following is done:

- `xtimer_period_wakeup()` is used to get 50 wake ups after 10 ms each
    - right before sleeping, the PTP clock is adjusted
    - right after the wake up, the PTP clock is read
- The difference between the two PTP timestamps should be the sum of the
  10 ms period length and the adjustment
    - The difference of the actual result and the expected result
      (10 ms + adjustment) is stored for each round
- The average, minimum, maximum, and variance (σ²) is calculated from these
  differences between actual period length and expected period length

A test run is considered a pass, if the average error is less than ± 250 ns
and minimum/maximum error are less than ± 500 ns.

Expected Output on Success
--------------------------

    main(): This is RIOT! (Version: <INSERT VERSION HERE>)
    Testing clock at speed 0 (~100.000 % of nominal speed): SUCCEEDED
    Testing clock at speed 32767 (~149.999 % of nominal speed): SUCCEEDED
    Testing clock at speed -32768 (~50.000 % of nominal speed): SUCCEEDED
    Testing clock at speed 1337 (~102.040 % of nominal speed): SUCCEEDED
    Testing clock at speed -1337 (~97.961 % of nominal speed): SUCCEEDED
    Testing clock at speed 42 (~100.064 % of nominal speed): SUCCEEDED
    Testing clock at speed -42 (~99.937 % of nominal speed): SUCCEEDED
    Testing clock at speed 665 (~101.015 % of nominal speed): SUCCEEDED
    Testing clock at speed -665 (~98.986 % of nominal speed): SUCCEEDED
    Testing clock adjustments for offset 0: SUCCEEDED
    Statistics: avg = 0, min = 0, max = 0, σ² = 0
    Testing clock adjustments for offset -1337: SUCCEEDED
    Statistics: avg = 0, min = 0, max = 0, σ² = 0
    Testing clock adjustments for offset 1337: SUCCEEDED
    Statistics: avg = 0, min = 0, max = 0, σ² = 0
    Testing clock adjustments for offset 2147483647: SUCCEEDED
    Statistics: avg = 0, min = 0, max = 0, σ² = 0
    TEST SUCCEEDED!

Note: If the values in the statistics of the clock adjustments differ to some
      degree, this might be due ISR overhead and jitter when reading the
      PTP timer from the periodic peripheral timer callback. The test will only
      fail if either the worst case offset or the variance are too big.
