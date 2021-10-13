# Test Application for `sys/atomic_utils`

## Design of the Test

This test application will launch one worker and one tester thread. The
worker thread will perform a specific operation (such as
`atomic_fetch_add_u32()`) over and over again on a single target variable, and
the tester thread occasionally interrupts to verify the target variable's value
is valid. If the variable has corrupted, this is reported.

This test works only *statistically*. Absence of detected corruption does not
guarantee, that a specific function indeed works correctly. (But a detected
corruptions guarantees that something is indeed broken.) However, the longer
the tests runs the higher the odds are that a malfunctioning implementation is
indeed caught corrupting memory.

## Types of Corruptions Tested For

### Lost Update

In the lost update the worker thread will perform an operation that has no
effect (addition, subtraction, binary or, and binary xor with `0` as
second parameter, or binary and with `0xff...`). The checker thread will
atomically increment the target value. If in the next iteration the of the
checker thread the value has changed, a corruption is detected. This could
happen e.g. by

```
Worker Thread           | Checker Thread    | Value of t
                        |                   |
                        | t++               | 1
reg1 = t                |                   | 1
reg1 += 0               |                   | 1
                        | t++               | 2
t = reg1                |                   | 1
```

Here, the read-modify-write sequence (`reg1 = t; reg1 += 0; t = reg1;`) has
been interrupted by the Checker Thread. The update of `t` (the atomic `t++`
operation) is afterwards lost, when the Worker Thread writes `reg1` into
`t`. Such a lost update proves that the read-modify-write operation was not
atomic.

Note: Only the `atomic_<op>_u<width>` family of functions must pass this test.
      A failure for the other families does ***not*** indicate an issue.

### Store Tearing

In the tearing test the worker thread will first initialize the target variable,
e.g. with zero. Then, a sequence of read-modify-write operations is performed,
e.g. 3 times `atomic_fetch_add_u16(&target, 0x5555)`. During this sequence, only
the target variable should contain on of the following values:
`0x0000`, `0x5555`, `0xaaaa`, and `0xffff`.

After each sequence is complete, the target variable will be atomically
re-initialized and the next sequence starts. If e.g. on AVR the write is
interrupted after only one byte is written (AVR is an 8-bit platform and only
can write 8 bits per store), e.g. a value of `0x55aa` or `0xaa55` might be
stored in the target variable. If such an value is observed, an atomic store
operation was torn apart into two parts and a memory corruption was detected.

Note: Both the `atomic_<op>_u<width>` and `semi_atomic_<op>_u<width>` families
      of functions must pass this test. A failure of the
      `volatile_<op>_u<width>` family of functions does ***not*** indicate an
      issue.

## Usage

The test will drop you into a shell. The welcome message and the help command
contain all information on how to use the test. In addition, `make test` will
run all tests that are expected to pass for one second each. This is hopefully
long enough to detect any issues. It is certainly not possible to run the test
longer in automated tests.

## Test Self Check

The test brings an alternative implementation of the `atomic_<op>_u<width>`
family of functions called `volatile_<op>_u<width>`. This implementation
incorrectly assumes that `volatile` provides atomic access. Thus, checking
this implementation should result in failures:

- The lost update test is expected to (eventually) fail for every platform
- The tearing test is expected for width bigger than the word size
    - Cortex-M7 is one exception: Due to instruction fusion two 32 bit writes
      can be issued in one CPU cycle, so that a 64 bit write can indeed be
      atomic for this platform. Thus, it could happen that no tearing failure
      is detected for the `volatile` implementation on Cortex-M7 at all.
