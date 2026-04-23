# About
This is a manual test application for the L6470 stepper motor driver using the 
X-NUCLEO-XIHM02A1 expansion board. The expansion board houses two of the L6470 
stepper motor drivers connected via daisy chain SPI.

# Usage
Enter the `make BOARD=??? flash` command in the `tests/driver_l6470/` folder. 
Connect the X-IHM02A1 Board to an STM32 Nucleo 64 board (F411RE or F072RB). 
The PB3 pin of the Nucleo 64 board is required as the SCLK pin to interface the 
X-IHM02A1 board.

# Results
This test application will initialize the two drivers and impose the following 
commands on both drivers:
1. Run motors at a constant speed.
2. Soft stop both motors.
3. Run both motors to a particular position.
4. Switch the step mode to 1/64 microsteps.
5. Move both motors 100 steps.
6. Hard Stop motor.
