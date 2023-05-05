Peripheral PTP Timer Test Application
=====================================

This tests setting the PTP timer to the following targets:

- one second into the future (using the absolute API)
- 1st of January 1970 0:00:00 (using the absolute API)
    - This is expected to fire right away according to the API
- 1 µs to 99µs into the future (using the relative API)

If all timeouts fire within ± 10 µs of the target time, the test succeeds.

Expected Output on Success
--------------------------

    main(): This is RIOT! (Version: <INSERT VERSION HERE>)
    TEST SUCCEEDED!
