# RTC & Display example

This example shows how you can use the HD44780 Display on the MCB2388 to show
the time of the Real-Time Clock.

The Potentiometer (POT1) and Button (INT0) are used as inputs to set the time.

## Flashing

Connect a USB to Serial adapter to COM0.
You can flash the demo application by entering

    make flash

The `mcb2388` board will be selected automatically.

## Entering the time

The Demo Application will prompt you to enter the Date and Time.

It will ask you to enter the current date and time with the modifiable time
component flashing.
Turn the potentiometer until the value is correct, then press the button (INT0)
to advance to the next value.
