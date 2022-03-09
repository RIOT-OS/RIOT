# posix_sleep test application

This test tests POSIX' `sleep()` and `usleep()`.
The test script also checks the sanity of the timings by comparing the overall
run time of the test with the host's time with an expected jitter of 15%.

## Usage
```
make flash test
```
