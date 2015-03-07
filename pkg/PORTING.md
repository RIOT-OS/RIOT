This directory provides some porting information for libraries and programs to
use with RIOT (to build an external module). If you'd like to add a package to
RIOT you need to add a directory with the name of your package to this directory.
Your directory should contain at least two files:

 * **patch.txt**- Your patch of the upstream application of the package to make
 it build with RIOT.
 * **Makefile**- A Makefile describing how to get the upstream application, apply
 the patch and how to build the package as a RIOT module.
 A rough template for several methods of acquiring a package
 is provided in Makefile.git, Makefile.http, and Makefile.svn
