Generation of `compile_commands.json`
=====================================

This tool can be used to generate `compile_commands.json` e.g. for code completion and linting in
IDEs. It relies on the build system providing the compilation details in bin dir. This tools is
best invoked indirectly via `make compile-commands` inside the application, which will first
generate the required files in the bin directory and then invoke this tool. For more details, check
the getting started page in the RIOT API documentation.
