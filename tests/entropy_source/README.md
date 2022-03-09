# About
This test compiles and runs entropy sources. The first `zero entropy` source can be run on
`native` and does not provide real entropy values, though, it tests execution of the main
module and its common components including the optional health tests. Additional sources are
requested subsequently. As indicated in the documentation of the [entropy module](../../sys/entropy_source/doc.txt),
entropy is vulnerable and specific hardware sources require a priori validation.
The `ADC noise` source requires the `periph_adc` feature of a board. Its properties can vary widely as
depicted in the documentation of the [`ADC noise`](../../sys/include/entropy_source/adc_noise.h)
modules and proper testing and parametrization need to take place for every single platform and ideally
environmental properties before deployment. The test simply initializes the ADC noise entropy source
with the default ADC pin, requests and dumps many samples with enabled health tests of the source and
conditioning.

# Expected results
The `zero entropy` is expected to indicate different errors after requesting more samples than
the cutoff value of the Repetition Count Test (NIST SP 800-90B 4.4.1) and the Adaptive
Proportion Test (NIST SP 800-90B 4.4.2). The expected output is:

## native

```
# main(): This is RIOT! (Version: <version>)
# Zero entropy single request 0/311 returned: 0
...
# Zero entropy single request 20/311 returned: -3
...
# Zero entropy single request 310/311 returned: -3
# Zero entropy single request 311/311 returned: -5
# Zero entropy request 64 Bytes: -5

```

## Board

The `ADC noise` source test should dump unpredictable values. The *von Neumann* conditioning internally
requests multiple samples so that subsequent values include bit changes. Thus, it has
a variable runtime. To explore this, one test requests a buffer with entropy values
and it measures the processing time. If no changes are in place, the conditioning function
will abort after exceeding a threshold.
Otherwise, no health test errors should be indicated. Please note that
missing errors do **not** reveal information about the quality of an entropy source.

```
# main(): This is RIOT! (Version: <version>)
# Zero entropy single request 0/311 returned: 0
...
# Zero entropy single request 20/311 returned: -3
...
# Zero entropy single request 311/311 returned: -5
# Zero entropy request 64 Bytes: -5
# ADC noise source entropy/sample: 65536 [2^16 * bit / sample]
# ADC noise source entropy/sample: 1 [bit / sample]
# 95
# 9f
# a5
# 01
# e1
# 3e
# 73
...
# ADC noise request 64 Bytes returned: 0. Time: <time>us
```