pkg_tinyvcdiff
==============

This test patches the binary file `source.bin` using `delta.bin` and reconstructs `target.bin`.

`delta.bin` has been created with open-vcdiff:

```
vcdiff delta -interleaved -dictionary source.bin <target.bin >delta.bin
```
