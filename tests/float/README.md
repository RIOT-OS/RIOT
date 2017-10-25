Expected result
===============
This application increases a non-even floating point number in steps of 0.1
starting from from 1234567.0 / 1024.0 ~= 1205.631835938. For each step it checks
if the sum minus its 'floored' values is less than 1.0 (which it should always be
by definition of `floor`).

Background
==========
This test was introduced due to an error for floating point handling in an older
newlib version.

The idea for this test is taken from:
http://sourceware.org/ml/newlib/2010/msg00149.html
