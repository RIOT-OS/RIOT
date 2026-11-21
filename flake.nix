{
  description = "RIOT OS development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    # Provides the ESP32 toolchains (xtensa-esp-elf and riscv32-esp-elf).
    # It keeps its own nixpkgs pin, as that package set is tested against it.
    nixpkgs-esp-dev.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs =
    {
      self,
      nixpkgs,
      nixpkgs-esp-dev,
      ...
    }:
    let
      lib = nixpkgs.lib;

      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = lib.genAttrs systems;

      pkgsFor =
        system:
        import nixpkgs {
          inherit system;
          # RIOT's Python requirements include ecdsa, which nixpkgs currently
          # marks as insecure (it is only used by the MCUboot tooling).
          config.permittedInsecurePackages = [ "python3.14-ecdsa-0.19.2" ];
          # adafruit-nrfutil bundles Nordic's proprietary DFU libraries.
          config.allowUnfreePackages = [ "adafruit-nrfutil" ];
        };

      espPackagesFor = system: nixpkgs-esp-dev.packages.${system};

      # Python 2.7 is only required by the GDB bundled with the MSP430
      # toolchain.  Current nixpkgs has dropped it, but the nixpkgs revision
      # pinned by nixpkgs-esp-dev still provides it.
      espPython27For =
        system:
        (import nixpkgs-esp-dev.inputs.nixpkgs {
          inherit system;
          config.permittedInsecurePackages = [ "python-2.7.18.12" ];
        }).python27;

      toolchainsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        import ./dist/nix/toolchains.nix {
          inherit pkgs;
          lib = pkgs.lib;
          python27 = espPython27For system;
          patchelf = pkgs.patchelfUnstable;
        };

      # Python base used by every target shell.
      pythonBasePackages =
        ps: with ps; [
          pexpect
          psutil
          pyserial
          twisted
          scapy
          (ps.callPackage ./dist/nix/python/riotctrl.nix { })
          (ps.callPackage ./dist/nix/python/pyhsslms.nix { })

          # Convenience
          pip
          rich
        ];

      # Extra Python modules that are only needed by a handful of package tests
      pythonPackageTestPackages =
        ps: with ps; [
          cbor
          cbor2
          colorama
          cryptography
          ecdsa
          pyasn1
          pycryptodome
          protobuf
          pybind11
          numpy
          joblib
          scipy
          scikit-learn
        ];

      pythonStaticPackages =
        ps: with ps; [
          flake8
          pyyaml
        ];

      pythonFullPackages =
        ps: pythonBasePackages ps ++ pythonPackageTestPackages ps ++ pythonStaticPackages ps;

      pythonEnvFor = system: packages: (pkgsFor system).python3.withPackages packages;

      # Host tools that are needed to build RIOT for any target.  The cross
      # toolchains are added by the individual shells.
      hostToolsFor =
        system:
        let
          pkgs = pkgsFor system;
          # gcc_multi provides the 32-bit multilib support needed by the
          # native32 board on x86_64.
          compilers =
            if system == "x86_64-linux" then
              [
                pkgs.gcc_multi
                pkgs.glibc_multi
              ]
            else
              [ pkgs.gcc ];
        in
        compilers
        ++ (with pkgs; [
          gnumake
          binutils
          gdb
          pkg-config
          cmake
          ninja
          meson
          autoconf
          automake
          libtool
          m4
          bison
          flex
          gettext
          texinfo
          ccache
          git
          (pkgs.callPackage ./dist/nix/git-cache.nix { })
          curl
          wget
          rsync
          unzip
          p7zip
          xz
          bzip2
          file
          which
          gawk
          gnused
          gnugrep
          findutils
          diffutils
          coreutils
          patchelf
          ncurses
        ]);

      staticPackagesFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          llvmPackages_20.clang-tools
          coccinelle
          cppcheck
          (pkgs.callPackage ./dist/nix/vera.nix { })
          uncrustify
          shellcheck
          codespell
          doxygen
          graphviz
          libxslt
          ncurses
        ];

      # Cross-LLVM uses the unwrapped Clang.  The Nix Clang wrapper is
      # host-specific and must not be used for cross builds.
      crossLlvmToolsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          llvmPackages_20.clang-unwrapped
          llvmPackages_20.bintools
          llvmPackages_20.llvm
          llvmPackages_20.lld
        ];

      # Some RIOT host tools are built with `env -i PATH=...`, so they cannot
      # see NIX_CFLAGS_COMPILE or PKG_CONFIG_PATH from the shell.  Provide a
      # small host compiler wrapper that puts the few required include and
      # library paths back on the command line.
      # @TODO: Find a solution around this
      makeHostCCFor =
        system: extraFlags:
        let
          pkgs = pkgsFor system;
          hostGcc = if system == "x86_64-linux" then pkgs.gcc_multi else pkgs.gcc;
          # The flag strings are multi-line for readability; fold them into a
          # single command line so that `exec` passes all arguments.
          flags = lib.replaceStrings [ "\n" ] [ " " ] extraFlags;
          script = pkgs.writeShellScript "riot-host-cc" ''
            exec ${hostGcc}/bin/gcc ${flags} "$@"
          '';
        in
        pkgs.runCommand "riot-host-cc" { } ''
          mkdir -p $out/bin
          ln -s ${script} $out/bin/cc
          ln -s ${script} $out/bin/gcc
        '';

      libusbCompatHostFlagsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        ''
          -isystem ${pkgs.libusb-compat-0_1.dev}/include
          -L${pkgs.libusb-compat-0_1}/lib
          -L${pkgs.libusb1}/lib
        '';

      # mspdebug is built from source and needs libusb-compat and readline.
      msp430HostFlagsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        lib.concatStringsSep " " [
          (libusbCompatHostFlagsFor system)
          "-isystem ${pkgs.readline.dev}/include"
          "-L${pkgs.readline}/lib"
        ];

      # EDBG is built from source and needs libudev.
      samHostFlagsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        ''
          -isystem ${pkgs.udev.dev}/include
          -L${pkgs.udev}/lib
        '';

      # The generic ARM shell can build both EDBG and the Teensy loader.
      armHostFlagsFor =
        system:
        lib.concatStringsSep " " [
          (samHostFlagsFor system)
          (libusbCompatHostFlagsFor system)
        ];

      # Kinetis' bundled teensy-loader-cli needs libusb-compat.
      kinetisHostFlagsFor = system: libusbCompatHostFlagsFor system;

      # The default shell contains all target tools, so use one wrapper with
      # the union of all required host flags.
      defaultHostFlagsFor =
        system:
        lib.concatStringsSep " " [
          (samHostFlagsFor system)
          (msp430HostFlagsFor system)
        ];

      # Toolchains and programmers grouped by the CPU family / board lineup
      # they are used for. (Based on TARGET_ARCH)
      targetToolsFor =
        system:
        let
          pkgs = pkgsFor system;
          esp = espPackagesFor system;
          toolchains = toolchainsFor system;

          armToolchain = [ pkgs.gcc-arm-embedded ];
          avrToolchain = with pkgs; [
            pkgsCross.avr.buildPackages.gcc
            pkgsCross.avr.buildPackages.binutils
          ];

          # The prebuilt vendor toolchains are x86_64 Linux binaries.
          riscvToolchain = lib.optionals (system == "x86_64-linux") [ toolchains.riscv ];
          msp430Toolchain = lib.optionals (system == "x86_64-linux") [ toolchains.msp430 ];
          esp8266Toolchain = lib.optionals (system == "x86_64-linux") [ toolchains.esp8266 ];

          # ESP32 has both Xtensa (ESP32/S2/S3) and RISC-V (C3/C6/H2) SoCs.
          esp32Toolchains = lib.optionals (system == "x86_64-linux" || system == "aarch64-linux") [
            esp.esp-idf-xtensa.tools.xtensa-esp-elf
            esp.esp-idf-xtensa.tools.xtensa-esp-elf-gdb
            esp.esp-idf-riscv.tools.riscv32-esp-elf
            esp.esp-idf-riscv.tools.riscv32-esp-elf-gdb
          ];

          serial = [ pkgs.picocom ];
        in
        {
          # Generic ARM environment
          # @TODO: include nordics nrfutil
          arm =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              pyocd
              stlink
              dfu-util
              bossa
              adafruit-nrfutil
              teensy-loader-cli
              cc2538-bsl
              stm32flash
              libusb1
            ]);

          rp-pico =
            armToolchain
            ++ riscvToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              libusb1
            ]);

          stm32 =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              stlink
              dfu-util
              stm32flash
            ]);

          nrf =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              pyocd
              adafruit-nrfutil
            ]);

          sam =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              bossa
              libusb1
            ]);

          kinetis =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              teensy-loader-cli
            ]);

          cc2538 =
            armToolchain
            ++ serial
            ++ (with pkgs; [
              openocd
              cc2538-bsl
            ]);

          cc26xx = armToolchain ++ serial ++ (with pkgs; [ openocd ]);

          lpc = armToolchain ++ serial ++ (with pkgs; [ openocd ]);

          gba =
            armToolchain
            ++ (with pkgs; [
              mgba
              SDL2
            ]);

          avr = avrToolchain ++ serial ++ (with pkgs; [ avrdude ]);

          msp430 = msp430Toolchain ++ serial ++ (with pkgs; [ mspdebug ]);

          esp = esp8266Toolchain ++ esp32Toolchains ++ serial ++ (with pkgs; [ esptool ]);

          riscv = riscvToolchain ++ serial ++ (with pkgs; [ openocd ]);

          # Cross-LLVM needs the target GCC toolchains for RIOT's system
          # include discovery and linking.  AVR and ESP do not support LLVM.
          # Native LLVM lives in `native`; the default shell uses
          # `clangDispatchFor` to select between this cross compiler and the
          # wrapped native one.
          llvm = crossLlvmToolsFor system ++ armToolchain ++ riscvToolchain;
        };

      nativeLlvmToolsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          llvmPackages_20.clang
          llvmPackages_20.llvm
          llvmPackages_20.bintools
          llvmPackages_20.lld
        ];

      # Both the native and the cross LLVM packages install a binary called
      # `clang`.  The default shell needs both: the Nix-wrapped native Clang
      # knows the host libc and resource headers, while the unwrapped cross
      # Clang can be used with `-target` without the wrapper's host flags.
      # This dispatcher picks the right one.  RIOT passes `-target` for cross
      # builds via makefiles/toolchain/llvm.inc.mk.
      clangDispatchFor =
        system:
        let
          pkgs = pkgsFor system;
          native = pkgs.llvmPackages_20.clang;
          cross = pkgs.llvmPackages_20.clang-unwrapped;
          dispatch = pkgs.writeShellScript "riot-clang-dispatch" ''
            case "$0" in
              *clang++) tool=clang++ ;;
              *) tool=clang ;;
            esac

            for arg in "$@"; do
              case "$arg" in
                -target|--target|-target=*|--target=*)
                  exec ${cross}/bin/$tool "$@"
                  ;;
              esac
            done

            exec ${native}/bin/$tool "$@"
          '';
        in
        pkgs.runCommand "riot-clang-dispatch" { } ''
          mkdir -p $out/bin
          ln -s ${dispatch} $out/bin/clang
          ln -s ${dispatch} $out/bin/clang++
        '';

      nativeExtraToolsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          qemu
          laze # Is this even used?
        ];

      nativeToolsFor = system: nativeLlvmToolsFor system ++ nativeExtraToolsFor system;

      # Tools used by network tests, CAN tests and a few package builds.
      miscToolsFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          minicom
          socat
          inetutils
          dtc
          gperf
          protobuf
          flatbuffers
          iproute2
          can-utils
        ];

      # stdenv exports the host compiler and binutils commands.  RIOT's
      # build system detects the toolchain from PATH, so these would
      # otherwise override the cross tools (notably OBJCOPY and OBJDUMP)
      # with their host counterparts.
      unsetToolsShellHook = ''
        unset CC CXX AR AS LD NM OBJCOPY OBJDUMP RANLIB SIZE STRIP
      '';

      # The Nix compiler wrapper adds host include paths for every
      # package in the shell.  Cross toolchains are plain vendor binaries
      # and ignore them, but nixpkgs' AVR/ARM wrappers would otherwise
      # feed host headers into the target compiler.
      commonShellHook = unsetToolsShellHook + ''
        unset NIX_CFLAGS_COMPILE NIX_CFLAGS_LINK
        # Make inside the shell must not try to re-enter Nix.
        export INSIDE_NIX=1
        export BUILD_IN_NIX=0
      '';

      # nixpkgs' sdl2-compat `sdl2-config` emits a bogus `-I/SDL2` entry.
      # Filter it out so `-Werror=missing-include-dirs` does not break RIOT.
      sdl2ConfigFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        pkgs.writeShellScriptBin "sdl2-config" ''
          out=$(${pkgs.SDL2.dev}/bin/sdl2-config "$@") || exit $?
          printf '%s\n' "$out" | ${pkgs.gnused}/bin/sed \
            -e 's#-I/SDL2##g' -e 's#  *# #g' -e 's#^ ##' -e 's# $##'
        '';

      mkDevShell =
        {
          system,
          name,
          packages,
          pythonPackages,
          # SDL is opt-in: only the native display/package-test shells need it.
          withSDL ? false,
          extraShellHook ? "",
        }:
        let
          pkgs = pkgsFor system;
        in
        pkgs.mkShell {
          inherit name;

          packages =
            hostToolsFor system
            ++ [ (pythonEnvFor system pythonPackages) ]
            ++ lib.optional withSDL (sdl2ConfigFor system)
            ++ packages;

          buildInputs = lib.optional withSDL pkgs.SDL2;

          shellHook =
            commonShellHook
            + lib.optionalString withSDL ''
              export PATH=${sdl2ConfigFor system}/bin:$PATH
            ''
            + extraShellHook
            + ''
              if [ -t 1 ]; then
                echo "Enabled RIOT Nix development shell: ${name} ❄️"
              fi
            '';
        };

      devShellsFor =
        system:
        let
          pkgs = pkgsFor system;
          targets = targetToolsFor system;
          fullPython = pythonFullPackages;
          basePython = pythonBasePackages;
          staticPython = ps: pythonBasePackages ps ++ pythonStaticPackages ps;
          isX86 = system == "x86_64-linux";

          armPackages = targets.arm ++ [
            pkgs.udev
            (makeHostCCFor system (armHostFlagsFor system))
          ];

          samPackages = targets.sam ++ [
            pkgs.udev
            (makeHostCCFor system (samHostFlagsFor system))
          ];

          kinetisPackages = targets.kinetis ++ [ (makeHostCCFor system (kinetisHostFlagsFor system)) ];

          msp430Packages = targets.msp430 ++ [ (makeHostCCFor system (msp430HostFlagsFor system)) ];

          # General native shell (gcc & llvm)
          nativeBase = mkDevShell {
            inherit system;
            name = "riot-native";
            packages = nativeLlvmToolsFor system;
            pythonPackages = basePython;
            withSDL = false;
          };

          # Full native environment, including QEMU for EMULATE=1
          # and SDL for the displayer driver tests
          nativeFull = mkDevShell {
            inherit system;
            name = "riot-native-full";
            packages = nativeToolsFor system;
            pythonPackages = fullPython;
            withSDL = true;
          };

          allPackages =
            targets.arm
            ++ targets.avr
            ++ targets.esp
            ++ targets.gba
            ++ targets.msp430
            ++ targets.riscv
            ++ crossLlvmToolsFor system
            ++ staticPackagesFor system
            ++ nativeExtraToolsFor system
            ++ miscToolsFor system
            ++ (with pkgs; [
              rustup
            ]);

          defaultPackages =
            allPackages
            ++ nativeLlvmToolsFor system
            ++ [
              (clangDispatchFor system)
              (makeHostCCFor system (defaultHostFlagsFor system))
            ];
        in
        {
          default = mkDevShell {
            inherit system;
            name = "riot-dev";
            packages = defaultPackages;
            pythonPackages = fullPython;
            withSDL = true;
            extraShellHook = ''
              export PATH=${makeHostCCFor system (defaultHostFlagsFor system)}/bin:$PATH
              export PATH=${clangDispatchFor system}/bin:$PATH
            '';
          };

          native = nativeBase;
          nativeFull = nativeFull;

          static-test = mkDevShell {
            inherit system;
            name = "riot-static-test";
            packages = staticPackagesFor system;
            pythonPackages = staticPython;
          };

          arm = mkDevShell {
            inherit system;
            name = "riot-arm";
            packages = armPackages;
            pythonPackages = basePython;
            extraShellHook = ''
              export PATH=${makeHostCCFor system (armHostFlagsFor system)}/bin:$PATH
            '';
          };

          rp-pico = mkDevShell {
            inherit system;
            name = "riot-rp-pico";
            packages = targets.rp-pico;
            pythonPackages = basePython;
          };

          stm32 = mkDevShell {
            inherit system;
            name = "riot-stm32";
            packages = targets.stm32;
            pythonPackages = basePython;
          };

          nrf = mkDevShell {
            inherit system;
            name = "riot-nrf";
            packages = targets.nrf;
            pythonPackages = basePython;
          };

          sam = mkDevShell {
            inherit system;
            name = "riot-sam";
            packages = samPackages;
            pythonPackages = basePython;
            extraShellHook = ''
              export PATH=${makeHostCCFor system (samHostFlagsFor system)}/bin:$PATH
            '';
          };

          kinetis = mkDevShell {
            inherit system;
            name = "riot-kinetis";
            packages = kinetisPackages;
            pythonPackages = basePython;
            extraShellHook = ''
              export PATH=${makeHostCCFor system (kinetisHostFlagsFor system)}/bin:$PATH
            '';
          };

          cc2538 = mkDevShell {
            inherit system;
            name = "riot-cc2538";
            packages = targets.cc2538;
            pythonPackages = basePython;
          };

          cc26xx = mkDevShell {
            inherit system;
            name = "riot-cc26xx";
            packages = targets.cc26xx;
            pythonPackages = basePython;
          };

          lpc = mkDevShell {
            inherit system;
            name = "riot-lpc";
            packages = targets.lpc;
            pythonPackages = basePython;
          };

          gba = mkDevShell {
            inherit system;
            name = "riot-gba";
            packages = targets.gba;
            pythonPackages = basePython;
          };

          avr = mkDevShell {
            inherit system;
            name = "riot-avr";
            packages = targets.avr;
            pythonPackages = basePython;
          };

          llvm = mkDevShell {
            inherit system;
            name = "riot-llvm";
            packages = targets.llvm;
            pythonPackages = basePython;
          };

          # On aarch64 this contains the ESP32 toolchains only; ESP8266 is
          # limited to the prebuilt x86_64 vendor toolchain.
          esp = mkDevShell {
            inherit system;
            name = "riot-esp";
            packages = targets.esp;
            pythonPackages = basePython;
          };
        }
        // lib.optionalAttrs isX86 {
          # These toolchains are prebuilt x86_64 Linux binaries.
          msp430 = mkDevShell {
            inherit system;
            name = "riot-msp430";
            packages = msp430Packages;
            pythonPackages = basePython;
            extraShellHook = ''
              export PATH=${makeHostCCFor system (msp430HostFlagsFor system)}/bin:$PATH
            '';
          };

          riscv = mkDevShell {
            inherit system;
            name = "riot-riscv";
            packages = targets.riscv;
            pythonPackages = basePython;
          };
        };

      helloWorldFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        pkgs.stdenv.mkDerivation {
          pname = "riot-hello-world-native64";
          version = "unstable";

          src = self;

          nativeBuildInputs = with pkgs; [
            gnumake
            python3
            gcc
            binutils
            unzip
            p7zip
          ];

          dontConfigure = true;

          # RIOT's helper scripts use `#!/usr/bin/env ...`.  The environment
          # in a Nix build does not provide /usr/bin/env, so rewrite the
          # interpreter lines for the build.
          postPatch = ''
            patchShebangs .
          '';

          buildPhase = ''
            runHook preBuild
            make -C examples/basic/hello-world BOARD=native64 \
              COLOR_ECHO=echo -j''${NIX_BUILD_CORES:-1} all
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            install -Dm755 examples/basic/hello-world/bin/native64/hello-world.elf \
              $out/bin/hello-world.elf
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "RIOT hello-world example for the native64 board";
            license = licenses.lgpl21Plus;
            platforms = platforms.linux;
          };
        };
    in
    {
      devShells = forAllSystems devShellsFor;

      packages = forAllSystems (system: {
        default = helloWorldFor system;
      });

      checks = forAllSystems (system: {
        hello-world-native64 = helloWorldFor system;

        formatting =
          (pkgsFor system).runCommand "check-nix-formatting"
            {
              nativeBuildInputs = [ (pkgsFor system).nixfmt ];
            }
            ''
              nixfmt --check \
                ${./flake.nix} \
                ${./dist/nix/toolchains.nix} \
                ${./dist/nix/vera.nix} \
                ${./dist/nix/git-cache.nix} \
                ${./dist/nix/python/riotctrl.nix} \
                ${./dist/nix/python/pyhsslms.nix}
              touch $out
            '';
      });

      formatter = forAllSystems (system: (pkgsFor system).nixfmt);
    };
}
