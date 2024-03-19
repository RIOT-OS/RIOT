Testing for Correct Stack Alignment
===================================

This test application asks the linker to align a stack to 128 B (assuming this
is the worst case alignment requirement). Not that features like the MPU may
result in much higher alignment requirements than the CPU actually has, thus
128 B is not crazy as it may sound. For each offset from 0 to 127 it will
then launch a thread using the aligned stack plus the current offset, thus
iterating over all possible stack alignments.

The test thread run `snprintf()` to format a double, compares the output with
the expected result, and exists to allow the subsequent thread to reuse the
stack. This is a good test for two reasons: Variadic functions (such as
`snprintf()`) on some platforms have different calling conventions that may
more easily trigger alignment issues, and an FPU may have a higher alignment
requirement than the CPU has.

The test is considered as passing if for all tested alignments the call to
`snprintf()` produces the correct result and no crash happens on the way.

Finally, the test script will collect the output of the stack consumptions and
give out the worst case penalty a user has to face
