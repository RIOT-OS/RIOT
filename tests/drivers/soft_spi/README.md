Expected result
===============
The test code transmits one byte with each SPI mode and a string over the
software SPI MOSI line. This can be verified with a Logic Analyzer connected to
the corresponding pins. When you see 0xa5 in all four SPI modes and the string
"Soft SPI Test String" in SPI mode 0 being transmitted via the CLK and MOSI pin,
the test is successful.

Notes
==========
- Because the module does not cover MISO inputs, neither does the test.
- The definition of SOFT SPI devices is done via the CFLAGS in the Makefile
