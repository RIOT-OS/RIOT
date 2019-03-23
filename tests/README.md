Running and creating tests                        {#running-and-creating-tests}
==========================

There are a number of tests included in RIOT. They are located in the
[tests folder](https://github.com/RIOT-OS/RIOT/tree/master/tests). These tests
allow basic functionality to be verified as well as provide an example of
usage.


Running automated tests
-----------------------

Some tests can be performed automatically. The test automation scripts are
defined in the `<test_application>/tests/` folder. They are written in python
and interact through the uart with the test application code running on a
board to do the validation. It is recommended to flash the board with the
test just before running it because some platforms cannot be reset while
testing.

From the test application directory run:

    BOARD=<board_of_your_choice> make flash test
