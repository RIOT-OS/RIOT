/**
 * @defgroup pkg   Packages
 * @{
 * @brief          External libraries and applications
 * @details        What are packages?  RIOT packages:
 *                 * pull a specific tested version most often from the (a) origin source
 *                 * patch them to match the RIOT architecture
 *                 * provide a specific tested configuration
 *                   (such configuration may enable or disable features)
 *                 * integrate the packages build process into the RIOT one
 *                 * they have RIOT tests and/or examples to test/build them
 *
 * Using packages
 * ==============
 * To add a package to the list of compiled modules you have to add it to the
 * `USEPKG` macro in your application's Makefile:
 *
 * ~~~~~~~~ {.mk}
 * USEPKG += <pkg_name>
 * ~~~~~~~~
 *
 * When the package can be built out-of-source, the source code of external
 * packages is fetched in a global location in:
 *
 * ~~~~~~~~ {.mk}
 * $(RIOTBASE)/build/pkg/$(PKG_NAME)
 * ~~~~~~~~
 *
 * When out-of-source build is not possible (for example because the package
 * build system doesn't allow it), the source code is fetched in the build
 * directory of the application under:
 *
 * ~~~~~~~~ {.mk}
 * $(BINDIR)/pkg/$(PKG_NAME)
 * ~~~~~~~~
 *
 * In this case, the source and the build directory are the same (currently,
 * this only applies to the micropython package).
 *
 * Porting an external library
 * ===========================
 * Structure of a package
 * ----------------------
 * This module provides porting information for libraries and applications to use
 * with RIOT (to build an external module). If you'd like to add a package to RIOT
 * you need to add a directory with the name of your package to this directory.
 * This directory should contain at least one file:
 *
 *  * **Makefile**: A Makefile describing how to get the upstream application,
 *    apply the patch and how to build the package as a RIOT module.
 *    A rough template for several methods of acquiring a package is provided in
 *    `pkg/Makefile.git` and `pkg/Makefile.http`.
 *
 * If your port or the package provide header files, you need to update the
 * `INCLUDES` variable in the package's `Makefile.include`:
 *
 * ~~~~~~~~ {.mk}
 * INCLUDES += -I$(RIOTPKG)/<pkg_name>/include # headers provided by the port
 * INCLUDES += -I$(PKGDIRBASE)/<pkg_name> # headers provided by the package
 * ~~~~~~~~
 *
 * Patch files can be included in a `patches` directory in the package dir.
 * These are applied to the upstream package to make it build with RIOT.
 *
 * Creating a patch with git
 * -------------------------
 * Assuming your upstream library resides in a git repository, you can create the
 * patch files as follows:
 *  * checkout the targeted version of the upstream application
 *  * create a new branch (e.g. `riot-port`)
 *  * conduct necessary changes (e.g. edit, add, or remove some files)
 *  * commit your changes using `git commit`
 *  * create the patch files using `git format-patch -n riot-port`
 *  * move the resulting patch files to the patches directory of your package.
 *
 * Packages outside of RIOTPKG
 * ---------------------------
 * It can be beneficial to create packages outside of the RIOT tree. For example
 * if one is working on new packages that aren't ready to be committed to
 * upstream or if an application needs its own unique packages. For this, one
 * can use the `EXTERNAL_PKG_DIRS` make variable. It works similar to the way
 * [external modules](src/creating-modules.html#modules-outside-of-riotbase) are
 * handled. In your application's Makefile, in addition to adding the package
 * name to `USEPKG` as shown above, add the path to a folder that contains your
 * external packages:
 *
 * ~~~~~~~~ {.mk}
 * EXTERNAL_PKG_DIRS += <PATH_TO_FOLDER_CONTAINING_PACKAGES>
 * ~~~~~~~~
 *
 * The path is allowed to be relative to the application's directory.
 *
 * ***NOTE:*** The name of an external package must be unique (both in regard to
 * other external packages, as well to native RIOT packages). Additionally, the
 * directory containing the packages must match the package name, e.g. package
 * `foo`must be located in `<PATH_IN_EXTERNAL_PKG_DIRS>/foo`.
 *
 * An example can be found in
 * [`tests/external_pkg_dirs`](https://github.com/RIOT-OS/RIOT/tree/master/tests/external_pkg_dirs)
 *
 * RIOT welcomes new and useful packages. If you'd like to share your work,
 * consider [contributing](https://github.com/RIOT-OS/RIOT/tree/master/CONTRIBUTING.md).
 *
 * @}
 */
