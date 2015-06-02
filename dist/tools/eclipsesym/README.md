cmdline2xml.sh
==============

Export all command line include paths and macro definitions to an XML file
suitable for import in Eclipse CDT.

Instrucions
-----------

The Eclipse project must be located at "/RIOT" inside your Eclipse workspace,
otherwise change cmdline2xml.sh accordingly (ECLIPSE_PROJECT_NAME=RIOT).

In the shell:

    cd to application directory (e.g. examples/hello-world)
    make eclipsesym

In Eclipse:

1. Open the project properties, menu Project->Properties
2. Select C/C++ General->Paths and Symbols
3. (optional) Click Restore Defaults to delete any existing macros and include paths
4. Click Import Settings...
5. Select `eclipsesym.xml` in your application directory and press Finish
6. Rebuild C/C++ index, menu Project->C/C++ Index->Rebuild

All conditional compilation and all include paths should now resolve properly
for your application.

The file `eclipsesym.xml` is specific to the application being built and may
differ depending on what modules are enabled and which platform is being built.
Make sure that everything is set up properly in your shell and that regular
`make all` works before running `make eclipsesym`
