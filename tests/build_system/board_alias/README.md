Test board aliases
==================

This tests in the CI whether all board aliases as defined in
`makefiles/board_alias.inc.mk` are handled correctly.

It does this by overriding the `info-boards-supported` target with all defined
board aliases. As long as the build succeeds, `BOARD` has been updated as
expected to a board with an existing folder in `boards/`
