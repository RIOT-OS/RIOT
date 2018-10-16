Motor driver test
======

Background
------

Test for the high level `motor_driver` driver.

Expected result
------

Should do an infinite loop :
    1) Both motors should turn at half pwm duty cycle speed in clowkwise
       direction.
    2) Both motors should turn at full pwm duty cycle speed in clowkwise
       direction.
    3) Both motors should brake.
    4) Both motors should turn at half pwm duty cycle speed in counter
       clowkwise direction.
    5) Both motors should turn at full pwm duty cycle speed in counter
       clowkwise direction.
    6) Both motors should brake.
