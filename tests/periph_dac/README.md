About
=====
This is a test application for a digital to analog converter (DAC).

This test application will initialize each configured DAC and one ADC (ADC_O) device to sample with
10-bit accuracy. The ADC is only initialized if there is one available on your board.

After initialization, values from 0 to 1000 are converted through the DACs in a loop. Shortly after the digital to analog conversion of one number, the ADC_0 samples its input signal. The numbers that are given to the DACs and the numbers that are sampled by the ADC were printed to the STDOUT. 

Usage
=====

a) Connect an oscilloscope to the DAC pins and look at the ten iteration signal levels

or

b) Connect the ADC input to the DAC outputs successively and compare if the sampled input value correlates with the printed output value at each DAC port.
