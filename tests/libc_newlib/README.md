About
=====

Terifies if newlib/newlib-nano is correctly included by the build system

At compile time, it checks that:

* newlib-nano header is used when 'newlib-nano' module is included
  * It defines `_NANO_FORMATTED_IO` macro
    https://sourceware.org/git/gitweb.cgi?p=newlib-cygwin.git;a=blob;f=newlib/newlib.hin;h=eadafc8a6a51ef7674c004a14777f6a4619115ee;hb=d34336767e45ee801ebb107e40abe4b1acbd3d14#l83
* newlib or newlib-nano is properly linked
  * `iprintf` is the same as `printf` or not as mentioned in:
    https://github.com/32bitmicro/newlib-nano-1.0/blob/f157c994b9a2c4bd8d0cfe9fe8fdd9cd54f8c63b/newlib/README.nano#L32

At runtime, it checks that:

* the same `iprintf` and `printf` test as at compile time


Without newlib, the test does nothing.
