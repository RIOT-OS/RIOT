## Kconfig features test

The objective of this test is to control the synchronization of features
provided by boards via `Makefile.features` and `Kconfig` files during the
migration process.

It also checks that multiple common variables (e.g. CPU_MODEL) have the same
value in the Makefiles and in Kconfig.

The test checks during compilation that both lists of features provided by
the board match. The `BOARD_WHITELIST` is used to test only the boards that have
their features modelled in Kconfig.
