# pqm4 ML-DSA-44 self-test

Exercises the `pkg/pqm4` package: keygen, sign, verify round-trip
for ML-DSA-44 (FIPS 204).

## Expected output

```
pqm4 ML-DSA-44 self-test
========================
  pk size: 1312 B
  sk size: 2560 B
  sig size: 2420 B
[1/3] crypto_sign_keypair ...
  OK
[2/3] crypto_sign ...
  OK (sm = 2453 B)
[3/3] crypto_sign_open ...
  OK (message recovered and verified)

ALL TESTS PASSED
```

## Tested on

- Raspberry Pi Pico 2 H (RP2350, Cortex-M33 @ 125 MHz)
- RIOT-OS 2026.04
