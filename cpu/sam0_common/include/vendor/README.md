# CMSIS from Atmel Software Foundation (ASF)

The include files in this directory tree are copied from Atmel
sources. Most of the sam0 files are from ASF (Atmel Software Foundation,
version 3.35.1). The SAMR30 files are from ASF 3.34.2. The SAML10 and
SAML11 files are from, so called, atpacks.

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

### sam0.h

A SAM based CPU should include `sam0.h` in this directory, which will
resolve  any CPU family specific includes required.

## SAML10/SAML11 files

Since 2019 the necessary variant files are available in atpacks at

    http://packs.download.atmel.com/

saml10: `Atmel.SAML10_DFP.1.0.142.atpack`
saml11: `Atmel.SAML11_DFP.1.0.91.atpack`

Each atpack has an include subdirectory with the files we copy into
RIOT. The files are copied unmodified.

### sam23.h

A SAML1x based CPU should include `sam23.h` in this directory, which will
resolve any CPU family specific includes required.

## Usage and porting for SAM based CPUs

If other CPUs are needed, copy the include files from ASF or atpack for the
respective  CPU family here and adapt `sam0.h` or `sam23.h` accordingly.
