mcufont
=======

The mcufont test application renders a string inside a pixel buffer after which
the pixel buffer is rendered using one ASCII Block symbol per pixel. This gives
a figlet-like effect.

A default black and white only font is included in this test. It has been
generated from the MCUFont tools and fonts.

When a display is present on the board with disp_dev support, the text string is
also shown on the display. In that case another font with aliasing support is
included in the build. This adds about 10 KiB to the build (including the
display driver)

Usage
=====

Compile the application and run the test.
