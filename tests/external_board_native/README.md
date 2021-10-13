Example with an external board
==============================

This tests an external board implementation.
The goal is to show using an external board that still needs to use the
'RIOT/boards' directory.

To allow automated testing by `murdock` this board is named 'native'.

As it wants to extend native, which is not a common board, some hacks must be
done as the 'board' module is implemented directly by the 'RIOT/boards/native'.

If you are running an 'ubuntu-bionic' you could even compile the example in
docker and run the test.

*Note:* This test uses still the deprecated `BOARDSDIR` variable instead of the
new `EXTERNAL_BOARD_DIRS` variable. This is by intent so that the CI can verify
backward compatibility is still working as intended.
