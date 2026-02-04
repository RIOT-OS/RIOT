PyCortexMDebug
==============

This is the RIOT integration of [PyCortexMDebug][], a GDB extension that allows
inspecting and interpreting memory of MCUs based on the info provided in SVD
files.

This integration adds PRs 58 and 59 on top of the upstream code. To get the
CMSIS database, run:

```sh
wget -O ~/.cache/cmdebug/cmsis-svd-data.zip https://github.com/cmsis-svd/cmsis-svd-data/archive/refs/heads/main.zip
```

Automating Loading of Correct SVD File
---------------------------------------

If `SVD_MODEL` and `SVD_VENDOR` are declared, the build system will inject
an `svd_load $(SVD_MODEL) $(SVD_VENDOR)` command into GDB, so that users don't
need to call that themselves.

Ideally, those variables are provided in `cpu/$(CPU)/Makefile.include`
by inferring it from the `$(CPU_MODEL)` variable each board already provides.

[PyCortexMDebug]: https://github.com/bnahill/PyCortexMDebug
