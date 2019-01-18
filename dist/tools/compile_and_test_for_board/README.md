Compile and Test one board
==========================


The `./compile_and_test_for_board.py` script can be used to run all compilation
and **automated** tests for one board, not the manual tests.


Usage
-----

    ./compile_and_test_for_board.py path_to_riot_directory board_name [results]

It prints the summary with results files relative to `results_dir/board` to have
a github friendly output.

Failures and all tests output are saved in files.
They can be checked with:

    find results/ -name '*.failed'
    find results/ -name 'test.success'


Implementation TODO
-------------------

The script was developed for being used outside of RIOT so does not currently
simply adapt to be used from within RIOT.

* Provide another script for new interface?
* Provide a RIOT/Makefile integration
* Save output files into `${BUILD_DIR}/output/compile_and_test` directory by
  default
* tox file to run `doctests/pylint/flake8`. Add tests.
* Implement the `board_is_supported`/`board_has_enough_memory`/`has_tests` to
  make targets instead of fiddling to get the value
