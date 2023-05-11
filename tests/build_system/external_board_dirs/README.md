Test for `EXTERNAL_BOARD_DIRS`
==============================

This test contains two directories containing external boards, namely
`external_board_dir_1` and `external_board_dir_2`. Each contains a symlink to
boards/native (so that this test does not need to maintain boards), the first
is named `native1` and the second `native2`. The variable `EXTERNAL_BOARD_DIRS`
is set to contain both directories.

This test succeeds if:

1. `make info-boards-supported` lists `native native1 native2`
2. Building works for all three boards, e.g. by
    a) `make BOARD=native`
    b) `make BOARD=native1`
    c) `make BOARD=native2`
