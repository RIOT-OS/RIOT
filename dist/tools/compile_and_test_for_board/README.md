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
