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

      pythonFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        pkgs.python3.withPackages (
          ps: with ps; [
            # RIOT build and test tooling
            pexpect
            psutil
            pyserial
            scapy
            (ps.callPackage ./dist/nix/python/riotctrl.nix { })
            (ps.callPackage ./dist/nix/python/pyhsslms.nix { })

            # Packages used by RIOT package tests
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

            # Static tests and documentation
            flake8
            pyyaml

            # Convenience
            pip
            rich
          ]
        );

      commonPackagesFor =
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
          (pythonFor system)
        ]);

      crossPackagesFor =
        system:
        let
          pkgs = pkgsFor system;
          esp = espPackagesFor system;
          toolchains = toolchainsFor system;
        in
        with pkgs;
        [
          gcc-arm-embedded
          pkgsCross.avr.buildPackages.gcc
          pkgsCross.avr.buildPackages.binutils
        ]
        ++ lib.optionals (system == "x86_64-linux") [
          toolchains.msp430
          toolchains.riscv
          toolchains.esp8266
        ]
        ++ lib.optionals (system == "x86_64-linux" || system == "aarch64-linux") [
          esp.esp-idf-xtensa.tools.xtensa-esp-elf
          esp.esp-idf-xtensa.tools.xtensa-esp-elf-gdb
          esp.esp-idf-riscv.tools.riscv32-esp-elf
          esp.esp-idf-riscv.tools.riscv32-esp-elf-gdb
        ];

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

      flashPackagesFor =
        system:
        let
          pkgs = pkgsFor system;
        in
        with pkgs;
        [
          openocd
          avrdude
          dfu-util
          pyocd
          adafruit-nrfutil
          mspdebug
          picocom
          minicom
          esptool
          libusb1
          socat
          inetutils
          qemu
          dtc
          gperf
          protobuf
          flatbuffers
        ]
        ++ lib.optionals (system == "x86_64-linux" || system == "aarch64-linux") [
          # These are currently not provided for aarch64-darwin
          bossa # Currently broken but should work again whenever they fix it
          stlink
          iproute2
          can-utils
        ];

      llvmPackagesFor =
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

      devShellsFor =
        system:
        let
          pkgs = pkgsFor system;

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
          '';
        in
        {
          default = pkgs.mkShell {
            name = "riot-dev";

            packages =
              commonPackagesFor system
              ++ crossPackagesFor system
              ++ staticPackagesFor system
              ++ flashPackagesFor system
              ++ llvmPackagesFor system
              ++ (with pkgs; [
                laze
                rustup
              ]);

            buildInputs = with pkgs; [
              SDL2
            ];

            shellHook = commonShellHook + ''
              if [ -t 1 ]; then
                echo "Enabled RIOT Nix development shell ❄️"
              fi
            '';
          };

          native = pkgs.mkShell {
            name = "riot-native";

            packages =
              commonPackagesFor system
              ++ (with pkgs; [
                qemu
                laze
                llvmPackages_20.clang
                llvmPackages_20.llvm
                llvmPackages_20.bintools
              ]);

            buildInputs = with pkgs; [
              SDL2
            ];

            shellHook = unsetToolsShellHook;
          };

          static-test = pkgs.mkShell {
            name = "riot-static-test";

            packages = commonPackagesFor system ++ staticPackagesFor system;

            shellHook = commonShellHook;
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
                ${./dist/nix/python/riotctrl.nix} \
                ${./dist/nix/python/pyhsslms.nix}
              touch $out
            '';
      });

      formatter = forAllSystems (system: (pkgsFor system).nixfmt);
    };
}
