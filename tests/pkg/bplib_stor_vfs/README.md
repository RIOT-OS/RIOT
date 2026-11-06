## bplib_stor_vfs

This test verifies that the bplib VFS storage implementation can store and
retrieve bundles. It also checks that its counting of bundles in storage and
size in storage is corrent, and not more than the reserved number of bundles can
be stored.

Since it uses the bplib shell command for this, this also serves as a good test
that all of the functionality works.

Overall tested code:
- bplib shell command
- core bplib operation (ingressing, bundle encoding)
- VFS storage

## Usage

Run `make all test`.
