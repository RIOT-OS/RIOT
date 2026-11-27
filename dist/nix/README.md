# Nix helper derivations

This directory contains the package definitions used by the RIOT Nix flake
(`flake.nix` in the repository root).

- `toolchains.nix` packages prebuilt vendor toolchains that are not available
  in nixpkgs (RIOT's MSP430 GCC, xPack RISC-V GCC, and the ESP8266 toolchain).
- `vera.nix` builds `vera++`
- `git-cache.nix` builds `git-cache-rs`
- `python/` contains Python packages that are missing from nixpkgs.

See the [Build in Nix guide](../../doc/guides/build-system/build-in-nix.md) for
usage instructions.
