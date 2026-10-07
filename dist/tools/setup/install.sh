#!/bin/bash
set -eu

ARCH="$(uname -m)"
OS="$(uname -s)"
SUDO=""; [ "$(id -u)" -ne 0 ] && SUDO="sudo"
BINFMT="/proc/sys/fs/binfmt_misc"
APT_UPDATED=0
NATIVE64_CROSS=1
NATIVE64_BINFMT=1

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]
 
Options:
  --skip-native-translation   Don't check for or install x86_64 translation
                              (Rosetta or QEMU x86_64 binfmt).
  --skip-native-cross         Don't check or install x86_64 cross-compilation
                              toolchain
  -h, --help                  Help!!
EOF
}

# install_pkgs "<apt packages>" "<pacman packages>" "<what these are for>"
# An empty list means "no package known for this package manager".
install_pkgs() {
    local apt_pkgs="$1" pacman_pkgs="$2" description="$3"

    if command -v apt-get >/dev/null 2>&1; then
        if [ -z "${apt_pkgs}" ]; then
            echo "🛑 Error: no 'apt' package known for ${description}" >&2
            echo "⚠️ Please install ${description} yourself." >&2
            return 1
        fi
        echo "⚙️ Installing ${apt_pkgs} using 'apt'"
        if [ "${APT_UPDATED}" -eq 0 ]; then
            $SUDO apt-get update -y || { echo "🛑 Error: 'apt-get update' failed" >&2; return 1; }
            APT_UPDATED=1
        fi
        $SUDO apt-get install -y ${apt_pkgs} \
            || { echo "🛑 Error: Installing ${description} failed" >&2; return 1; }
    elif command -v pacman >/dev/null 2>&1; then
        if [ -z "${pacman_pkgs}" ]; then
            echo "🛑 Error: no 'pacman' package known for ${description}" >&2
            echo "⚠️ Please install ${description} yourself (maybe from the AUR)." >&2
            return 1
        fi
        echo "⚙️ Installing ${pacman_pkgs} using 'pacman'"
        $SUDO pacman -S --needed --noconfirm ${pacman_pkgs} \
            || { echo "🛑 Error: Installing ${description} failed" >&2; return 1; }
    else
        echo "🛑 Error: Install script could not find 'apt' or 'pacman' package managers" >&2
        echo "⚠️ Please install ${description} yourself." >&2
        return 1
    fi
    echo "✅ Installed ${description}"
}

ensure_x86_64_cross_compilation() {
    if command -v x86_64-linux-gnu-gcc >/dev/null 2>&1; then
        return 0
    fi
    # No official pacman package for an x86_64 cross compiler on ARM
    install_pkgs \
        "gcc-x86-64-linux-gnu libc6-dev-amd64-cross libc6-amd64-cross" \
        "" \
        "x86_64 cross-compilation toolchain"
}

has_binfmt() {
    ls "$BINFMT" 2>/dev/null | grep -q "^$1"
}

ensure_x86_64_binfmt() {
    # Maybe we're in a VM on macOS and have rosetta
    if has_binfmt rosetta; then
        echo "✅ x86_64 binaries can run: Rosetta binfmt available"
        return 0
    fi
    if has_binfmt qemu-x86_64; then
        echo "✅ x86_64 binaries can run: QEMU binfmt already available"
        return 0
    fi

    echo "⚙️ Installing x86_64 QEMU binfmt"
    install_pkgs \
        "qemu-user-binfmt binfmt-support" \
        "qemu-user-static qemu-user-static-binfmt" \
        "QEMU binfmt packages" || return 1

    mountpoint -q "$BINFMT" || $SUDO mount -t binfmt_misc none "$BINFMT"
    $SUDO systemctl restart systemd-binfmt 2>/dev/null || true

    if has_binfmt qemu-x86_64; then
        echo "✅ x86_64 binaries can run: QEMU binfmt installed"
    else
        echo "🛑 Error: no qemu-x86_64 binfmt handler registered after installing QEMU" >&2
        return 1
    fi
}

can_build_m32() {
    echo 'int main(void){return 0;}' | gcc -m32 -x c - -o /dev/null 2>/dev/null
}

ensure_native_works() {
    case "$ARCH" in
        x86_64|amd64)
            if ! can_build_m32; then
                echo "ℹ️ Need multilib for BOARD=native32"
                install_pkgs \
                    "gcc-multilib" \
                    "lib32-glibc lib32-gcc-libs" \
                    "32-bit (multilib) compiler support" || return 1
            fi
            ;;
        i386|i486|i586|i686|x86|x86_32|i86pc)
            echo "⚠️ BOARD=native64 unsupported"
            return 0
            ;;
        aarch64|arm64)
            echo "⚠️ BOARD=native64 is experimental under ARM Linux"
            echo "ℹ️ Need to cross-compile and translate x86_64 BOARD=native64 binaries"
            if [ "${NATIVE64_CROSS}" = "1" ]; then
                ensure_x86_64_cross_compilation || return 1
            else
                echo "ℹ️ Skipping x86_64 cross-compilation toolchain"
            fi 
            if [ "${NATIVE64_BINFMT}" = "1" ]; then
                ensure_x86_64_binfmt || return 1
            else
                echo "ℹ️ Skipping x86_64 binfmt"
            fi 
            ;;
        *)
            echo "🛑 Error: $ARCH unsupported, cannot run native32/native64 apps" >&2
            return 1
            ;;
    esac
}

main() {
    echo "ℹ️ ARCH=$ARCH OS=$OS"

    case "$OS" in
        Linux)
            ;;
        Darwin)
            if [ "${ARCH}" != "arm64" ]; then
                echo "⚠️ macOS $ARCH is unsupported!" >&2
                exit 1
            fi
            echo "⚠️ Building and running natively on macOS are not supported yet" >&2
            exit 1 ;;
        *)
            echo "🛑 Error: unknown option: $1" >&2; usage >&2
            exit 1 ;;
    esac

    install_pkgs \
        "build-essential make python3-serial python3-psutil wget unzip git openocd gdb-multiarch esptool podman-docker clangd clang" \
        "base-devel python-pyserial python-psutil wget unzip git openocd gdb esptool podman-docker clang" \
        "required packages" || return 1
    ensure_native_works
}

check_01() {
    case "$2" in
        0|1) ;;
        *)
            echo "🛑 Error: $1 needs the value 0 or 1, got '$2'" >&2; 
            exit 1
            ;;
    esac
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --native64-cross=*)
            if [ "${OS}" != "Linux" ]; then
                echo "🛑 Error: --native64-arm-cross and --native64-arm-binfmt are only supported on Linux" >&2
                exit 1
            fi
            NATIVE64_CROSS="${1#*=}"
            check_01 --native64-cross "${NATIVE64_CROSS}"
            ;;
        --native64-binfmt=*)
            if [ "${OS}" != "Linux" ]; then
                echo "🛑 Error: --native64-arm-cross and --native64-arm-binfmt are only supported on Linux" >&2
                exit 1
            fi
            NATIVE64_BINFMT="${1#*=}"
            check_01 --native64-binfmt "${NATIVE64_BINFMT}"
            ;;
        --native64-cross|--native64-binfmt)
            check_01 $1 ""
            exit 1 
            ;;
        -h|--help)                 usage; exit 0 ;;
        *)                         echo "🛑 Error: unknown option: $1" >&2; usage >&2; exit 1 ;;
    esac
    shift
done

main
