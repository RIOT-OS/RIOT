Satisfy LGPL - a small example
==============================
This is an example showing how you can use your own library to replace or add specific proprietary functions to RIOT not violating LGPL.

The basic example just uses a `mutex` and greets you, thats it.
The proprietary modifications are applied to `./RIOT/core/mutex.c mutex_lock()` function.  
If you run the build with `make` followed by `make term` you will be using the original unmodified RIOT functions.

##First lets test if our proprietary functions just work:
```
make with_source
```
does this for us.  
It applies a patch (`mutex_apply.patch`) to `./RIOT/core/mutex.c`, builds RIOT with the changes and creates `mutex_revert.patch` to undo the modifications.
Entering `make term` will show us that the modifications were applied.

##Now lets make our proprietary library:
```
make PROPRIETARY_BUILD=1 create_library
```
does this for us.
If `mutex_revert.patch` is not present it creates it to enable undo the modifications.
It also applies a patch (`mutex_apply.patch`) to `./RIOT/core/mutex.c`, builds RIOT with the changes and finally creates a `my_cool_lib.a` for us, whereas the name is specified by `LIBNAME` in the `Makefile`.  


##Lets link it
To link the created library we enter:
```
make PROPRIETARY_BUILD=1 with_library
```
which renames `library.c` to `library.c_off` to exclude it from the build process.  
Then it builds RIOT linking the `my_cool_lib.a`.
Again, entering `make term` will show us that the modifications were applied.

##Summing up
Providing a proprietary library created with a specific RIOT version and a hook patch allows us to build an executable to verify if LGPL has been respected.  
The binary resulting from the build process should be always the same if the build conditions are equal, e.g. a specific compiler version and libc libraries.  

In this example I would ship:
- the `mutex_apply.patch` including the hook to our library functions
- the `library.h` providing the interfaces to the hooks
- the `Makefile` to ease up the build
- the build target information, i.e. `native` for this example
- the used compiler version, i.e. `gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1)`
- the RIOT version hash, i.e. `319f1b25ae4c8146aee0bf171bf7271b3854a233`
- the md5 checksum of the built binary with the linked library, i.e. `7e80ce210c52ca481c7228830f1d2942  bin/native/satisfy_lgpl.elf`
- and of course the used library `my_cool_lib.a`

This information should allow anyone to reproduce the build and to verify if RIOT has been modified other than the changes from `mutex_apply.patch`.

##To clean thing up we enter
```
make unmodify
```
to revert the `mutex_apply.patch` changes by applying the generated patch `mutex_revert.patch` and rename the `library.c_off` to `library.c`
