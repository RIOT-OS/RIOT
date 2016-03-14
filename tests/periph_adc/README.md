Expected result
===============
When running this test, you should see the samples of all configured ADC lines
continuously streamed to std-out.

Background
==========
This test application will initialize each configured ADC lines to sample with
10-bit accuracy. Once configured the application will continuously convert each
available channel and print the conversion results to std-out.

For verification of the output connect the ADC pins to known voltage levels
and compare the output.
