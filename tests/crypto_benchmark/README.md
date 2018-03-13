# Crypto Benchmark
In this test, software and hardware crypto implementations are compared.

Hardware crypto is not required to run this test.

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
On native target:

```
Starting benchmarks.
CRYPTO 3DES: completed 329238 ops in 10 seconds, 32923 ops/sec.
CRYPTO AES128: completed 34540312 ops in 10 seconds, 3454031 ops/sec
CRYPTO TWOFISH: completed 3357247 ops in 10 seconds, 335724 ops/sec
CRYPTO MD5: completed 32832281 ops in 10 seconds, 3283228 ops/sec
CRYPTO SHA1: completed 10454394 ops in 10 seconds, 1045439 ops/sec
CRYPTO SHA256: completed 7055956 ops in 10 seconds, 705595 ops/sec
Benchmarks done!
```

On a Silicon Labs SLSTK3401A @ 40 MHz:

```
Starting benchmarks.
CRYPTO 3DES: completed 2799 ops in 10 seconds, 279 ops/sec.
CRYPTO AES128: completed 227649 ops in 10 seconds, 22764 ops/sec
CRYPTO TWOFISH: completed 12385 ops in 10 seconds, 1238 ops/sec
CRYPTO MD5: completed 327992 ops in 10 seconds, 32799 ops/sec
CRYPTO SHA1: completed 163460 ops in 10 seconds, 16346 ops/sec
CRYPTO SHA256: completed 123892 ops in 10 seconds, 12389 ops/sec
HWCRYPTO AES128: completed 1324448 ops in 10 seconds, 132444 ops/sec
HWCRYPTO AES256: completed 1126714 ops in 10 seconds, 112671 ops/sec
HWCRYPTO SHA1: completed 348721 ops in 10 seconds, 34872 ops/sec
HWCRYPTO SHA256: completed 335838 ops in 10 seconds, 33583 ops/sec
Benchmarks done!
```
