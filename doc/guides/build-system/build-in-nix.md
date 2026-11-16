---
title: Build in Nix
description: Building RIOT in a Nix-managed development environment
---

RIOT provides a [Nix flake](https://nixos.wiki/wiki/Flakes) that contains the
cross toolchains, static analysis tools, and host utilities needed to build
RIOT. It is an alternative to the [Docker build
environment](/build-system/build-in-docker/) and does not require root
privileges or a Docker daemon.

The flake lives in the repository root (`flake.nix`); the helper derivations
are under `dist/nix/`. Nix support is still highly experimental.

## Requirements

- Nix with flakes enabled (Nix 2.4 or newer)
- A Linux host: `x86_64-linux` has the full toolchain set, `aarch64-linux`
  supports the host, Arm, AVR, and ESP32 toolchains

The Docker image remains the recommended option for exact reproduction of the
official CI environment.

## Entering the Development Shell

From the RIOT repository root:

```shell
nix develop
```

The default shell contains all supported cross toolchains and the static test
tools. Smaller shells are available for specific tasks:

```shell
nix develop .#native       # host / native board only
nix develop .#static-test  # static tests only
```

The `native` shell also contains the regular (wrapped) LLVM toolchain, so
`TOOLCHAIN=llvm` can be used for native builds. The default shell contains
LLVM for cross compilation.

## Building

Once inside the shell, use `make` as usual:

```shell
make -C examples/basic/hello-world BOARD=native64
make -C examples/basic/hello-world BOARD=samr21-xpro
make -C examples/basic/hello-world BOARD=arduino-uno
```

For non-interactive use, pass the command to `nix develop`:

```shell
nix develop --command make -C examples/basic/hello-world BOARD=native64
```

`BUILD_IN_DOCKER` is not needed and should not be set inside the Nix shell.

The toolchain versions are intentionally not identical to the ones in
[riotdocker](https://github.com/RIOT-OS/riotdocker). The Docker image pins the
exact versions used by CI; the Nix shell tracks nixpkgs instead. For RISC-V,
MSP430, and ESP8266 the same prebuilt vendor toolchains as in riotdocker are
used.

## Static Tests

The `static-test` shell contains the tools used by `make static-test`:

```shell
nix develop .#static-test --command ./dist/tools/ci/static_tests.sh
```

The script expects a Git base branch (or `CI_BASE_BRANCH`) as in CI.

## Updating the Environment

The flake pins nixpkgs and the ESP toolchains in `flake.lock`. Update them with:

```shell
nix flake update
```

Format the Nix files with:

```shell
nix fmt
```

Validate the flake and build the `native64` smoke test with:

```shell
nix flake check
```

## Limitations

- `native32` is only available on `x86_64-linux`.
- The RISC-V, MSP430, and ESP8266 toolchains are prebuilt `x86_64-linux`
  binaries; they are not available on `aarch64-linux`.
- The ESP8266 toolchain's GDB is not included. It depends on Python 2.7 and an
  old ncurses ABI that are not available in current nixpkgs. Use an external
  GDB for ESP8266 debugging.
- `c2rust` is not packaged in nixpkgs and has to be installed with `cargo`
  (see above).
- The picolibc variants built into riotdocker are not part of the Nix shell.
  Use the Docker image for builds that require `PICOLIBC=1`.
- Flashing and debugging real hardware still requires the usual udev rules
  and permissions for the debug adapter.
