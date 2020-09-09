## Rationale

- Kconfig with the `--sync-deps` flag creates a directory tree in the $(BINDIR) folder
- This directory tree contains dummy header files for each Kconfig configuration symbol
- `fixdep` is a tool from the linux kernel [1] to rewrite dependency
  files (`*.d`) to include the corresponding dummy header files
- This way, `make` is able to draw correct dependencies between
  compilation units and configuration symbols
- => `--sync-deps` and `fixdep` enable incremental compilations, i.e.,
  only those compilation units are rebuilt, which are affected by
  configuration changes.

[1] https://github.com/torvalds/linux/blob/83bdc7275e6206f560d247be856bceba3e1ed8f2/scripts/basic/fixdep.c

## Changes to the original script

The patch file `fixdep_riot.patch` contains a few necessary changes
and enhancements to `fixdep.c` in order to make the generated `*.d`
dependency files compatible with our build system.

- The input parameters of `fixdep` are changed to:
    ```
    ./fixdep <dependency_file> <target> <sync_deps_dir>
    ```
- The hardcoded `include/config` folder location in `fixdep.c` is
  changed to be configurable (passed from the command line)
- Our Kconfig integration does not use the `_MODULE` suffix for
  configurations. This is reflected in the parsing functionality of
  `fixdep.c`
