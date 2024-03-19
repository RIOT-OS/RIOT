# About

This application is intended for testing Feetech TTL bus.
It is especially designed to test Feetech SCS15 servomotors.

# Usage

To have the list of available commands :
```
help
```

You will need to initialize one UART at 1000000 baudrate (if the servomotor is in factory configuration) :
```
init 1 1000000
```

To ping the servomotor :
```
ping 1
```

Be careful ! If 2 servomotors with the same ID are connected on the same bus, you will have no response.
Factory configuration ID is 1, you need to change this to connect an other servo.

To scan every connected servomotors (IDs from 0 to 253) :
```
scan
```

To read a servo register :
```
read 1 PRESENT_POSITION
```

To move a servo, you need to enable the torque and set the goal position [0-1024] :
```
write 1 ENABLE_TORQUE 1
write 1 GOAL_POSITION 512
```
