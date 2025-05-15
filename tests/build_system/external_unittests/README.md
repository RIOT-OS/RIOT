External Unit Tests
===================

This test apps provides two trivial unit tests: One provided internally in the
folder of this unit tests apps and one provided externally via a variable.
Since the test only validates correct operation of the build system, there is
little value in running it on more than one board. For this reason, the app is
limited to be build only for native / native64.

Implementation Details
----------------------

The two tests contain a reference to a variable of the other tests, so that
the app will not link if only if both of the unit tests are available at
link time (or none). In addition, the python test runner will check that two
 tests are executed (to rule out that none was available).
