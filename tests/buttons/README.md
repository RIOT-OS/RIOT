# On-Board Button Test

This tests initializes all on-board buttons available, currently up to 4 in
total. Each button is configured as an input with interrupt on button press
or release depending on the configured TEST_FLANK.

# Expected Result

When a button is pressed (and/or released) an interrupt fires which results in
a message indicating which button was used. The test naturally qualifies for
manual testing only. Nevertheless, it prints "[SUCCESS]" after all buttons
were initialized successfully or "[FAILED] <msg>" on error, so it can be used
for limited automated testing as well.
