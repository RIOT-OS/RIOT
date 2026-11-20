# Toolchains that are not available (or not usable) from nixpkgs.
#
# RIOT needs a couple of vendor toolchains that are distributed as prebuilt
# x86_64 Linux binaries.  They are wrapped here so that they can be used from
# the Nix development shell like any other package.
{
  lib,
  pkgs,
  # Python 2 is only needed by the GDB shipped with the MSP430 toolchain.
  # It is taken from the nixpkgs revision used by nixpkgs-esp-dev.
  python27,
  # xPack's binaries have a non-standard ELF layout that the patchelf version
  # in nixpkgs (0.15) corrupts.  Use a newer patchelf for autoPatchelfHook.
  patchelf,
}:

let
  inherit (pkgs)
    autoPatchelfHook
    fetchurl
    stdenv
    ;

  # The prebuilt toolchains are dynamically linked against a handful of
  # libraries that are not part of the default stdenv.
  runtimeLibs = with pkgs; [
    stdenv.cc.cc.lib
    zlib
    ncurses5
    expat
    xz
    gmp
    mpfr
    libmpc
  ];

  metaCommon = {
    platforms = [ "x86_64-linux" ];
    sourceProvenance = [ lib.sourceTypes.binaryNativeCode ];
  };
in
{
  # RIOT's own MSP430 toolchain, the same one used by riotdocker.
  msp430 = stdenv.mkDerivation rec {
    pname = "riot-msp430-elf-gcc";
    version = "10.1.0-18";

    src = fetchurl {
      url = "https://github.com/RIOT-OS/toolchains/releases/download/10.1.0-18-20200722112854-64162e7/riot-msp430-elf-10.1.0-18.tgz";
      hash = "sha256-/xDeb9RWdVehz2HnwWtVpqIe25gAr+icIijBnaOjKw8=";
    };

    nativeBuildInputs = [ autoPatchelfHook ];
    buildInputs = runtimeLibs ++ [ python27 ];

    installPhase = ''
      runHook preInstall

      mkdir -p $out
      cp -a msp430-elf/10.1.0-18/. $out/

      runHook postInstall
    '';

    meta = metaCommon // {
      description = "GNU MSP430 toolchain for RIOT";
      homepage = "https://github.com/RIOT-OS/toolchains";
      license = lib.licenses.gpl3Plus;
      mainProgram = "msp430-elf-gcc";
    };
  };

  # xPack's RISC-V toolchain.  nixpkgs' cross RISC-V GCC does not ship the
  # newlib-nano multilib variants that RIOT uses, so use the same toolchain
  # that riotdocker pins.
  riscv = stdenv.mkDerivation rec {
    pname = "xpack-riscv-none-elf-gcc";
    version = "13.2.0-2";

    src = fetchurl {
      url = "https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v${version}/xpack-riscv-none-elf-gcc-${version}-linux-x64.tar.gz";
      hash = "sha256-UlRa+5ACAPv2X+BffOcJC4pCxkCR9PXUPK5rto6iQ0o=";
    };

    nativeBuildInputs = [
      patchelf
      autoPatchelfHook
    ];

    installPhase = ''
      runHook preInstall

      mkdir -p $out
      cp -a ./. $out/

      runHook postInstall
    '';

    meta = metaCommon // {
      description = "xPack GNU RISC-V Embedded GCC";
      homepage = "https://xpack.github.io/riscv-none-elf-gcc/";
      license = with lib.licenses; [
        gpl3Plus
        lgpl3Plus
      ];
      mainProgram = "riscv-none-elf-gcc";
    };
  };

  # Prebuilt ESP8266 toolchain used by RIOT and riotdocker.  The bundled GDB
  # is removed: it is linked against Python 2.7 and an old ncurses ABI, neither
  # of which is available in current nixpkgs.
  esp8266 = stdenv.mkDerivation rec {
    pname = "xtensa-esp8266-elf-gcc";
    version = "5.2.0-696257c";

    src = fetchurl {
      url = "https://github.com/gschorcht/xtensa-esp8266-elf/archive/696257c2b43e2a107d3108b2c1ca6d5df3fb1a6f.tar.gz";
      hash = "sha256-yVuEjBXXJTXMHdrIZq3pddOw3JrPzjzTrktPyjU9aEw=";
    };

    sourceRoot = "xtensa-esp8266-elf-696257c2b43e2a107d3108b2c1ca6d5df3fb1a6f";

    nativeBuildInputs = [ autoPatchelfHook ];
    buildInputs = with pkgs; [
      stdenv.cc.cc.lib
      zlib
      ncurses5
      expat
      xz
    ];

    # The sysroot contains dangling lib32/lib64 compatibility symlinks.
    dontCheckForBrokenSymlinks = true;

    installPhase = ''
      runHook preInstall

      mkdir -p $out
      cp -a ./. $out/
      rm -f $out/bin/xtensa-esp8266-elf-gdb \
            $out/bin/xtensa-esp8266-elf-gdb-add-index \
            $out/bin/xtensa-esp8266-elf-gdb-add-index-py3

      runHook postInstall
    '';

    meta = metaCommon // {
      description = "Xtensa ESP8266 toolchain for RIOT";
      homepage = "https://github.com/gschorcht/xtensa-esp8266-elf";
      license = with lib.licenses; [
        gpl2Plus
        gpl3Plus
        lgpl21Plus
        lgpl3Plus
      ];
      mainProgram = "xtensa-esp8266-elf-gcc";
    };
  };
}
