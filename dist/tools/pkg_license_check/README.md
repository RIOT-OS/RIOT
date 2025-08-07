# Package License Checker

Listing and verifying the license expressions declared for external
packages that are part of the RIOT OS distribution.

Included scripts:

- `validate_spdx_license_expressions.py`: validate newline separated SPDX license
  expressions provided via stdin, see https://spdx.org/licenses/ for valid
  license identifiers and links to the expression syntax
- `get_package_license_identifiers.sh`: collect all legal status and license
  declarations for all packages and output a list
- `check_packages.sh`: glue script that pipes all declared package licenses
  through the SPDX validator

Running the SPDX validator script requires the `spdx-tools` package as per
`requirements.txt`.
