# Do the following to calibrate your HX711

1. Start with "HX711_PARAM_OFFSET 0" and "HX711_PARAM_SCALE 1" in your Makefile
2. Flash your microcontroller with these values
3. Remove all weights on your scale.
Only keep the things on your load cells which should be included for finding the zero point.
4. Check the returned values in your terminal and use it for the offset.
5. Flash again with the new offset
6. Place a reference weight on your connected load cell.
7. Get the average value and use HX711_PARAM_SCALE = AVG_VALUE / REFERENCE_WEIGHT
8. You should use different reference weights. For example 1g, 2g, 5g, 10g, 20g.
Depending on the accuracy you need.
