# CMSIS from Atmel Software Foundation (ASF)

The include files in the directory tree are copied from ASF (verion 3.35.1).

The directory tree was copied "as is" from ASF path `sam/utils/cmsis/` and its
structure is as follows:

    cmsis
    ├── sam3x
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
    ├── sam4s
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

# Usage and porting for SAM based CPUs

SAM based CPU should include `sam.h` in this directory, which will resolve any
CPU family specific includes required.

Currently only SAM3X and SAM4S based CPUs are supported, i.e., only their CMSIS
header files are copied from the ASF. If other CPUs are needed copy CMSIS files
from the ASF for the respective CPU family here and adapt `sam.h` accordingly.
