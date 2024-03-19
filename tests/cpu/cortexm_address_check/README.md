# Cortex-M check for memory address validity

## Introduction
Cortex-M3/M4/M7-based MCUs allow to check memory address validity
by temporarily blocking BusFault handler.

Validity check can be used to determine RAM/flash/EEPROM sizes,
peripherals availability, etc., to create firmware that runs
effectively on different MCUs without recompiling.

NB: Cortex-M0 and Cortex-M0+ don't have BusFault events, all
bus errors escalate to HardFault immediately.