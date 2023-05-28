# wolfCrypt Ed25519 verify routine test

This is a demo of a signature verification using wolfCrypt.

wolfCrypt is part of [wolfSSL](https://www.wolfssl.com) which provides several different cryptographic services.

In this test, a text message is signed using Edwards-curve digital signature algorithm,
using the Ed25519 signature scheme.

## How it works

The message and its signature are hardcoded in the example. To create a new
signed message, you may use the host-application `ed25519_sign_msg` under `tools/`,
which generates a new EdDSA key pair, and produces a signature for the test message.

The demo verifies the authenticity of the message via `wc_ed25519_verify_msg`.
