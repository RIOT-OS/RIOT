Background
==========

Test for the high level servo driver.

Expected result
===============

A servo connected to `PWM_0` channel 0 should move back and forth inside the
angle -90 degrees to +90 degrees, approximately.

Using a scope should show a varying pulse length between 1000 us to 2000 us
long. The requested frequency is 100 Hz, but due to hardware limitations it
might not be possible to achieve the selected frequency. The pulse width
should, however, remain the same, only the frequency of pulses (and hence the
duty cycle) should differ.
