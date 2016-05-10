# Crypto Benchmark
In this example, software and hardware crypto implementations are compared.

## Benchmarks
Currently, the following benchmarks are executed (depending on support):

* Software
  * AES128
  * 3DES
  * TWOFISH
  * SHA256
* Hardware
  * AES128
  * AES256
  * SHA1
  * SHA256

## Sample output
On an SLSTK3401A @ 40 MHz:

```
main(): This is RIOT! (Version: 2016.07-devel-110-g4a133-MacBook-Pro-van-Bas.local)
Starting benchmarks.
CRYPTO 3DES: 2786 ops in 10 seconds.
CRYPTO AES128: 225474 ops in 10 seconds.
CRYPTO TWOFISH: 12332 ops in 10 seconds.
CRYPTO SHA256: 123354 ops in 10 seconds.
HWCRYPTO AES128: 1431591 ops in 10 seconds.
HWCRYPTO AES256: 1247614 ops in 10 seconds.
HWCRYPTO SHA1: 354406 ops in 10 seconds.
HWCRYPTO SHA256: 346080 ops in 10 seconds.
Benchmarks done!
```
