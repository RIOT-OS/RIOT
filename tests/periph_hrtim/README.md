Expected result
===============
You should be able to initialize all the timings units with a given
frequency and a given deadtime, then manage duty-cycle and phase
positioning of 5 or 6 complementary PWM outputs.

You should also be able to perform some basic HRTIM operations and
produce the oscillograms of the HRTIM cookbook's section 2.

Background
==========
Test for the low-level HRTIM driver.

## Complementary pwm outputs with deadtime and phase positioning

5 phase interlaced pwm with 100ns deadtime, 50% duty-cycle example.

Shell:
```shell
    init 0 100000 100
   The effective frequency is 100000Hz
   The period is set to 46080
    pwm_set 0 0 23040 0
    pwm_set 0 1 23040 9216
    pwm_set 0 2 23040 18432
    pwm_set 0 3 23040 27648
    pwm_set 0 4 23040 36864
```
## an4539 - HRTIM cookbook - 2 Basic operating principles

### 2.1 Single PWM generation
C:
```c
    uint32_t freq;
    uint16_t period;

    /* Initialize the master timer and TIMD at 100KHz frequency */
    freq = KHZ(100);
    hrtim_init_mstr(0, &freq);
    period = hrtim_init_tu(0, TIMD, &freq);

    /* Set duty cycle to 50% */
    hrtim_cmp_set(0, TIMD, CMP1xR, period / 2);

    /* TD1 output set on TIMD period and reset on TIMD CMP1 event */
    hrtim_set_cb_set(0, TIMD, OUT1, PER);
    hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);

    /* Start Timer D */
    hrtim_cnt_en(0, (MCEN | TDCEN));

    /* Enable TD1 output */
    hrtim_out_en(0, TIMD, OUT1);
```
Shell:
```shell
    init_mstr 0 100000
    init_tu 0 3 100000
    cmp_set 0 3 1 23040
    set_cb_set 0 3 1 2
    rst_cb_set 0 3 1 3
    cnt_en 0 17
    out_en 0 3 1
```
### 2.2 Generating multiple PWMs
C:
```c
    uint32_t freq;
    uint16_t period;

    /* Initialize the master timer and TIMD at 100KHz frequency */
    freq = 33333;
    hrtim_init_mstr(0, &freq);
    freq = KHZ(100);
    period = hrtim_init_tu(0, TIMD, &freq);

    /* Set CMP1 to 25% and CMP2 to 75% of period */
    hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);
    hrtim_cmp_set(0, TIMD, CMP2xR, period * 3 / 4);

    /* TD1 output set on TIMD period and reset on TIMD CMP1 event*/
    hrtim_set_cb_set(0, TIMD, OUT1, PER);
    hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);
    /* TD2 output set on TIMD CMP2 and reset on TIMD period event*/
    hrtim_set_cb_set(0, TIMD, OUT2, CMP2);
    hrtim_rst_cb_set(0, TIMD, OUT2, PER);

    /* Timer A initialization at 33KHz */
    freq = 33333;
    period = hrtim_init_tu(0, TIMA, &freq);

    /* Set duty cycles to 25% */
    hrtim_cmp_set(0, TIMA, CMP1xR, period / 4);
    hrtim_cmp_set(0, TIMA, CMP2xR, period / 2);
    hrtim_cmp_set(0, TIMA, CMP3xR, period * 3 / 4);

    /* TA1 output set on TIMA period and reset on TIMA CMP1 event*/
    hrtim_set_cb_set(0, TIMA, OUT1, PER);
    hrtim_rst_cb_set(0, TIMA, OUT1, CMP1);
    /* TA2 output set on TIMA CMP2 and reset on TIMA CMP3 event*/
    hrtim_set_cb_set(0, TIMA, OUT2, CMP2);
    hrtim_rst_cb_set(0, TIMA, OUT2, CMP3);

    /* Enable TA1, TA2, TD1 and TD2 outputs */
    hrtim_out_en(0, TIMA, OUT1 | OUT2);
    hrtim_out_en(0, TIMD, OUT1 | OUT2);

    /* Start Master, Timer A and Timer D counters*/
    hrtim_cnt_en(0, (MCEN | TACEN | TDCEN));

    /* Timer A and Timer D counters reset on Master period event */
    hrtim_rst_evt_en(0, TIMA, RST_MSTPER);
    hrtim_rst_evt_en(0, TIMD, RST_MSTPER);
```

### 2.3 Generating PWM with other timing units and the master timer
C:
```c
    uint32_t freq;
    uint16_t period;

    /* Initialize the master timer, set period for 100002Hz frequency
     * and duty cycle to 50% */
    freq = KHZ(100) + 2;
    period = hrtim_init_mstr(0, &freq);
    hrtim_cmp_set(0, MSTR, MCMP1R, period / 2);

    /* Initialize timer D, set period for 100kHz frequency
     * and duty cycle to 50% */
    freq = KHZ(100);
    period = hrtim_init_tu(0, 3, &freq);
    hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);

    /* TD1 output set on TIMD period and reset on TIMD CMP1 event*/
    hrtim_set_cb_set(0, TIMD, OUT1, PER);
    hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);

    /* TD2 output set on master timer period and reset on master CMP1 event*/
    hrtim_set_cb_set(0, TIMD, OUT2, MSTPER);
    hrtim_rst_cb_set(0, TIMD, OUT2, MSTCMP1);

    /* Enable TD1 and TD2 outputs */
    hrtim_out_en(0, TIMD, OUT1 | OUT2);

    /* Start Master Timer and Timer D counters*/
    hrtim_cnt_en(0, MCEN | TDCEN);
```
### 2.4 Arbitrary waveform generation
C:
```c
    uint32_t freq;
    uint16_t period;

    /* Initialize the master timer and TIMD at 100KHz frequency */
    freq = KHZ(100);
    hrtim_init_mstr(0, &freq);
    period = hrtim_init_tu(0, TIMD, &freq);

    /* Set edge timings */
    hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);
    hrtim_cmp_set(0, TIMD, CMP2xR, period * 3 / 8);
    hrtim_cmp_set(0, TIMD, CMP3xR, period / 2);

    /* TD1 toggles on TIMD period, CMP1 and CMP2 event */
    hrtim_set_cb_set(0, TIMD, OUT1, PER | CMP1 | CMP2);
    hrtim_rst_cb_set(0, TIMD, OUT1, PER | CMP1 | CMP2);

    /* TD2 output set on TIMD PER and CMP2 and reset on TIMD CMP1
     * and CMP3 event */
    hrtim_set_cb_set(0, TIMD, OUT2, PER | CMP2);
    hrtim_rst_cb_set(0, TIMD, OUT2, CMP1 | CMP3);

    /* Enable TD1 and TD2 outputs */
    hrtim_out_en(0, TIMD, OUT1 | OUT2);

    /* Start Timer D */
    hrtim_cnt_en(0, TDCEN);
```
