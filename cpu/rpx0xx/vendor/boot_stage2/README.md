How to Generate
===============

Run `make` to create the boot stage 2 variants. The sources, the linker script
and the `pad_checksum` script are taken from the `picosdk` package, which is
fetched automatically if needed. Only stubs for the headers that the Pico SDK
generates with CMake are kept in `include/`.

The resulting `boot2_w25q080_padded_checksummed.S` has to be copied to
`cpu/rpx0xx/` manually. Ideally, the boot loader needs to never be touched.
