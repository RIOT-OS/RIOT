genconfigheader.sh
------------------

Usage: `genconfigheader.sh <output_header_file> [CFLAGS]`

Generate a build configuration header from CFLAGS arguments

 - Arguments on the form `-Dmacro_name=macro_value` will be converted to
   the form `#define macro_name macro_value`
 - Arguments given on the form `-Dmacro_name` will be converted to the form `#define macro_name 1`
 - The output file will be overwritten if it already exists _and_ the new output file's contents differs from the old file.

By not replacing the output file on every run, make can still use the file
modification times for dependency calculations.
