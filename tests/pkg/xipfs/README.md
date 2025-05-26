# The eXecute In-Place File System tests

## Description

This application is a best effort to test the return codes of system
calls provided by `xipfs`. It accomplishes this by executing these calls
in various environments, ensuring that each system call returns the
expected error codes. However, further in-depth tests need to be done.

## Tested cards

`xipfs-tests` is expected to be compatible with all boards that feature
addressable NVM. However, only the `DWM1001` board has been tested and
is confirmed to function correctly.

## Funding

The `xipfs-tests` project is part of the TinyPART project funded by the
MESRI-BMBF German-French cybersecurity program under grant agreements
n°ANR-20-CYAL-0005 and 16KIS1395K.
