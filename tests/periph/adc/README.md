Expected result
===============
When running this test, you should see the samples of all configured ADC lines
continuously streamed to std-out.

Background
==========
This test application will initialize each configured ADC lines to sample with
6 to 16-bit accuracy. Once configured the application will continuously convert
each available channel with each available resolution and print the conversion
results to std-out.

Not all MCUs support all resolutions and unsupported resolutions should be
printed as -1.

For verification of the output connect the ADC pins to known voltage levels
and compare the output.
