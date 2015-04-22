This directory provides some porting information for libraries and programs to
use with RIOT (to build an external module). If you'd like to add a package to
RIOT you need to add a directory with the name of your package to this directory.
Your directory should contain at least two files:

 * **One or more patch files** - Your patches of the upstream application of
   the package to make it build with RIOT.
 * **Makefile**- A Makefile describing how to get the upstream application,
   apply the patch and how to build the package as a RIOT module.
   A rough template for several methods of acquiring a package is provided in
   Makefile.git, Makefile.http, and Makefile.svn

### Creating a patch with git

Assuming your upstream application resides in a git repository, you can create
the patch files as follows:
 * checkout the targeted version of the upstream application
 * conduct necessary changes (e.g. edit, add, or remove some files)
 * commit your changes using `git commit`
 * create the patch files using `git format-patch -n HEAD~N` where `N` is the
   number of commits you did
 * move the resulting patch files to the corresponding subfolder of pkg
