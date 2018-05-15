# xtimer_configuration test application

This test tests `_xtimer_tsleep32()` and `xtimer_periodic_wakeup` against the
timings of of the xtimer.
And agains the timings of the external host (PC).

This test can be used to adjust `XTIMER_SHOOT_OVERHEAD`, `XTIMER_OVERHEAD`,
`XTIMER_BACKOFF` and `XTIMER_ISR_BACKOFF`.
This is to ensure that the xtimer interrupt is only as long as neccessary.

## Usage
```
make BOARD=<Board Name> flash test
```

## Find right values

1. Configure parameter in board.h as follow:
`XTIMER_ISR_BACKOFF` has to be twice `XTIMER_OVERHEAD`
```
#define XTIMER_OVERHEAD (50)
#define XTIMER_SHOOT_OVERHEAD (0)
#define XTIMER_BACKOFF_OVERHEAD (1)
#define XTIMER_BACKOFF  (2*(XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD))
#define XTIMER_ISR_BACKOFF (XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD)
```

run the test.
If the output is as follows
```
xxxx-xx-xx xx:xx:xx,xxx - INFO # 240876 243436 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 27 ticks
Increase value for XTIMER_SHOOT_OVERHEAD or XTIMER_OVERHEAD 27
...
xxxx-xx-xx xx:xx:xx,xxx - INFO # 266798 266845 slept for 47 ticks (376 us) expected 47 ticks, diff 14 ticks
Increase value for XTIMER_BACKOFF_OVERHEAD 14
```

Add the new value for `XTIMER_SHOOT_OVERHEAD` and `XTIMER_BACKOFF_OVERHEAD` to the board.h and set `XTIMER_OVERHEAD` to zero.

```

#define XTIMER_OVERHEAD (0)
#define XTIMER_SHOOT_OVERHEAD (27)
#define XTIMER_BACKOFF_OVERHEAD (14)
#define XTIMER_BACKOFF  (2*(XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD))
#define XTIMER_ISR_BACKOFF (XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD)
```

2. run test again


If the output is as follows

```
xxxx-xx-xx xx:xx:xx,xxx - INFO # 240876 243436 slept for 2560 ticks (20480 us)expected 2560 ticks, diff 33 ticks
Increase value for XTIMER_SHOOT_OVERHEAD or XTIMER_OVERHEAD 33
```

Set `XTIMER_OVERHEAD` to new value.
Set 33 as value for `XTIMER_OVERHEAD` and
`XTIMER_BACKOFF` to `2*(XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD)`.
Also set `XTIMER_ISR_BACKOFF` to `XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD`.

```
#define XTIMER_OVERHEAD (33)
#define XTIMER_SHOOT_OVERHEAD (27)
#define XTIMER_BACKOFF_OVERHEAD (14)

#define XTIMER_BACKOFF  (2*(XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD))
#define XTIMER_ISR_BACKOFF (XTIMER_OVERHEAD+XTIMER_SHOOT_OVERHEAD)
```

3. Run the test again and check if there are values which are far off or values which are smaller than expected.



now run `tests/xtimer_usleep_short` and `tests/xtimer_usleep`.

The expectet result should be similar to this.

```
xxxx-xx-xx xx:xx:xx,xxx - INFO # main(): This is RIOT! (Version: 2018.07-devel-197-g413845-josua-VBox-pr/xtimer_core)
xxxx-xx-xx xx:xx:xx,xxx - INFO # Running test 5 times with 9 distinct sleep times
xxxx-xx-xx xx:xx:xx,xxx - INFO # Please hit any key and then ENTER to continue
a
a
xxxx-xx-xx xx:xx:xx,xxx - INFO # Testing Periodic xtimer with absolute timestamp.
xxxx-xx-xx xx:xx:xx,xxx - INFO # 237061 239621 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 239621 242181 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 242181 244741 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 244741 247301 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 247301 249861 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 249861 252421 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 252421 254981 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 254981 257541 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 257541 260101 slept for 2560 ticks (20480 us) expected 2560 ticks, diff 0 ticks
Increase value for XTIMER_SHOOT_OVERHEAD or XTIMER_OVERHEAD 0
xxxx-xx-xx xx:xx:xx,xxx - INFO # Testing Periodic xtimer with relative timestamp.
xxxx-xx-xx xx:xx:xx,xxx - INFO # 285054 285567 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 285567 286080 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 286080 286593 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 286593 287106 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 287106 287619 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 287619 288132 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 288132 288645 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 288645 289158 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 289158 289671 slept for 513 ticks (4104 us) expected 511 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # Testing Periodic xtimer spin.
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313129 313226 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313224 313321 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313319 313416 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313414 313511 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313509 313606 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313604 313701 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313699 313796 slept for 97 ticks (776 us) expected 95 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313794 313890 slept for 96 ticks (768 us) expected 95 ticks, diff 1 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 313889 313985 slept for 96 ticks (768 us) expected 95 ticks, diff 1 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # Testing xtimer sleep 32 spin.
xxxx-xx-xx xx:xx:xx,xxx - INFO # 336566 336614 slept for 48 ticks (384 us) expected 47 ticks, diff 1 ticks
Increase value for XTIMER_BACKOFF_OVERHEAD 1
xxxx-xx-xx xx:xx:xx,xxx - INFO # 338980 339030 slept for 50 ticks (400 us) expected 49 ticks, diff 1 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 341396 341499 slept for 103 ticks (824 us) expected 96 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 343897 409440 slept for 65543 ticks (524344 us) expected 65536 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 412098 413105 slept for 1007 ticks (8056 us) expected 1000 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 415633 417985 slept for 2352 ticks (18816 us) expected 2345 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 420546 470553 slept for 50007 ticks (400056 us) expected 50000 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 473211 573218 slept for 100007 ticks (800056 us) expected 100000 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 575941 699404 slept for 123463 ticks (987704 us) expected 123456 ticks, diff 7 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # Testing XTIMER_ISR_BACKOFF.
xxxx-xx-xx xx:xx:xx,xxx - INFO # thread(): waiting going to sleep!
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704206 744216 slept for 40010 ticks (320080 us) expected 40000 ticks, diff 10 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704223 744261 slept for 40038 ticks (320304 us) expected 40028 ticks, diff 10 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704242 744302 slept for 40060 ticks (320480 us) expected 40056 ticks, diff 4 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704263 744350 slept for 40087 ticks (320696 us) expected 40084 ticks, diff 3 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704285 744400 slept for 40115 ticks (320920 us) expected 40112 ticks, diff 3 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704308 744452 slept for 40144 ticks (321152 us) expected 40140 ticks, diff 4 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704333 744505 slept for 40172 ticks (321376 us) expected 40168 ticks, diff 4 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704176 744226 slept for 40050 ticks (320400 us) expected 40029 ticks, diff 21 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # 704190 744579 slept for 40389 ticks (323112 us) expected 40387 ticks, diff 2 ticks
xxxx-xx-xx xx:xx:xx,xxx - INFO # Test ran for 4267104 us
```