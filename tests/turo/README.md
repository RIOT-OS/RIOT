# TURO (Test Utils Result Output) Test

This shows a non-trival example of how to use the TURO module as a
testing abstraction layer.

The test is written with only TURO commands allowing the underling output to
be changed as needed depending on the interpreter. This means that the test
will not need to be changed if output is changed. If the test results are
output as json and the binary is too large, the TURO can be switched to CBOR
to save space. The interpreter should also switch to a CBOR parser and the
test should not need to be changed.

This should keep tests more stable, which is particularly useful for automated
tests.