Expected result
===============
When running this test, you should see the averaged samples of `CONFIG_TEST_ADC_LINE` lines
continuously streamed to std-out. The ADC line is sampled at full speed and aggregates
measurements every `CONFIG_TEST_ADC_BUF_SAMPLES` samples, for `CONFIG_TEST_ADC_SLEEP_SEC`
seconds. After the sampling period, the number of actually processed samples should roughly
match the number of theoretically possible samples.

Background
==========
This test application will initialize 1 configured ADC lines to sample with
12-bit accuracy over DMA in two buffers. Once configured the application will
continuously convert each available sample according to `ADC_VREF_MV`
and print the averaged conversion results to std-out.

For verification of the output connect the ADC pins to known voltage levels
and compare the output.
