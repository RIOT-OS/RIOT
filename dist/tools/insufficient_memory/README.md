add_insufficient_memory_board.sh
------------------

Usage: `add_insufficient_memory_board.sh <board_name>`

Updates `Makefile.ci` to include `<board_name>` if the memory of the board is not sufficient for the test/example.

For this the script will build every test and example to see if the result would fit into the memory of the specified
board. If not the corresponding `Makefile.ci` is updated automatically.
