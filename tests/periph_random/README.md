Expected result
===============
This test outputs a sequence of random bytes, starting with one, then two and so on, until 20 random bytes are printed. Then the application sleeps for a second and starts over.

Background
==========
This test was introduced to test the implementation of the low-level random number generator driver. For most platforms the implementation is based on hardware CPU peripherals.
