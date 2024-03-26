# CMSIS from Atmel Software Foundation (ASF)

The include files in this directory tree are copied from Atmel
sources. Most of the sam0 files are from ASF (Atmel Software Foundation,
version 3.35.1). The SAML10 and SAML11 files are from, so called, atpacks.

## The sam0 files

The directory tree was copied "as is" from ASF path `sam0/utils/cmsis/` and
its  structure is as follows:

    cmsis
    ├── samd21
    │   ├── include
    │   │   ├── component
    │   │   ├── instance
    │   │   └── pio
    │   └── source
    │       ├── gcc
    │       └── iar
    .
    .
    .
    ├── samr21
    │   ├── include
    │   │   ├── component
    │   │   ├── instance
    │   │   └── pio
    .   └── source
    .       ├── gcc
    .       └── iar

However, as only the header files in `include` and its subfolders are required,
all `source` folders are removed.

Be aware that if you want to make changes to any file in this tree that the
changes will be lost when a new ASF release is going to be used.

### C++ compatibility (with LLVM)

Many of the header files where generated with an outdated version of SVDConv
that adds the `__I` qualifier to anonymous bit fields, which `clang++` won't
compile. Run the script `fix_headers.sh` in this directory whenever you
add or update vendor header filers until Microchips starts using a fixed version
of SVDConv.

### compatibility with tinyDTLS and `<endian.h>`

Both tinyDTLS as well as `<endian.h>` expect `LITTLE_ENDIAN` to be a magic
number to compare the byte order (as provided by the compiler) against. The
vendor header files provide the macro with the wrong magic number without
ever using this macro. We can just strip using the `fix_headers.sh` script
in this directory.

### sam0.h

A SAM based CPU should include `sam0.h` in this directory, which will
resolve  any CPU family specific includes required.

## SAMD10 files

samd10: `Atmel.SAMD10_DFP.1.1.77.atpack`

## SAMD20 files

samd20: `Atmel.SAMD20_DFP.1.3.165.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

## SAMD21 files

samd21: `Atmel.SAMD21_DFP.1.3.395.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

## SAML10/SAML11 files

Since 2019 the necessary variant files are available in atpacks at

    http://packs.download.atmel.com/

saml10: `Atmel.SAML10_DFP.1.0.142.atpack`
saml11: `Atmel.SAML11_DFP.1.0.91.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

## SAML21 files

saml21: `Atmel.SAML21_DFP.1.2.125.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

## SAMR21 files

samr21: `Atmel.SAMR21_DFP.1.1.72.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The following replacements were done for compatibility with newlib:

    find -name '*.h' -exec sed -ie 's/_U(/_U_(/g' {} \;
    find -name '*.h' -exec sed -ie 's/_L(/_L_(/g' {} \;

## SAMR30 files

samr30: `Atmel.SAMR30_DFP.1.1.35.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT.

The PIN_($pin)_EIC_EXTINT_NUM macos and integer literal macros (_L_() etc)
were added manually.
It is expected for those to appear in the next atpack release by
Microchip.

If *_EIC_EXTINT_NUM macros are missing after an update, GPIO interrupts
not work. See 93d536f76 for how to generate them.

## SAMR34 files

samr34: `Atmel.SAMR34_DFP.1.0.11.atpacks`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

### sam23.h

A SAML1x based CPU should include `sam23.h` in this directory, which will
resolve any CPU family specific includes required.

## Usage and porting for SAM based CPUs

If other CPUs are needed, copy the include files from ASF or atpack for the
respective  CPU family here and adapt `sam0.h` or `sam23.h` accordingly.
