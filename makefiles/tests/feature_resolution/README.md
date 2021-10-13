Simple Unit Tests for Feature Resolution
========================================

This folder contains a Makefile that runs a set of unit tests for the feature resolution logic.
Each unit test is defined in a file named `tests-<NAME_OF_TEST>.mk` in which the inputs and the
expected outputs are defined. Finally, the include `Makefile.test`, which will perform the tests.

The tests can be run simply by typing `make`. Also, `make static-test` at the root of the repo will
run these tests and, hence, these tests are also executed on each CI run.

Adding new Test Cases
---------------------

Just create a new `tests-<NAME_OF_NEW_TEST>.mk` file and start with the contests of an existing
test case and change the inputs and expected outputs accordingly.
