# Overview

This test application tests all the components of the crypto module in RIOT.
Specifically these are:

* ChaCha. Test vectors from [draft-strombergson-chacha-test-vectors-00].
* Poly1305. Test vectors from [draft-nir-cfrg-chacha20-poly1305-06].
* ChaCha20-Poly1305. Test vectors from [rfc7539].
* AES-CBC. Test vectors from [SP 800-38C].
* AES-CCM. Test vectors from [RFC3610], [SP 800-38C], [Wycheproof].
* AES-CTR. Test vectors from [SP 800-38C].
* AES-ECB. Test vectors from [SP 800-38C].
* AES-OCB. Test vectors from [RFC7253].

To build the test application run

```
make
```

To execute the test run

```
make term
```

[draft-nir-cfrg-chacha20-poly1305-06]: https://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06#appendix-A.3
[draft-strombergson-chacha-test-vectors-00]: https://tools.ietf.org/html/draft-strombergson-chacha-test-vectors-00
[rfc7539]: https://tools.ietf.org/html/rfc7539#appendix-A
[SP 800-38C]: http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
[RFC3610]: https://tools.ietf.org/html/rfc3610
[Wycheproof]: https://github.com/google/wycheproof/blob/master/testvectors/aes_ccm_test.json
[RFC7253]: https://tools.ietf.org/html/rfc7253#appendix-A