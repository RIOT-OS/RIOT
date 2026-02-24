## RIOT Documentation

RIOT has various documentation sources in this folder, including:

### Starlight / Guide Site

Starlight is the main documentation site for RIOT, located at
[guide.riot-os.org](https://guide.riot-os.org/),
the `starlight` directory itself mostly only holds the source files for the site,
excluding most guides which are stored within `guides`.

If you wish to contribute new guides or find out more, please see
[Creating Guides & How Starlight Works](https://guide.riot-os.org/misc/how_to_doc/).

### Doxygen / API Site

Doxygen is the home of all the API documentation,
located at [api.riot-os.org](https://api.riot-os.org/).
The `doxygen` directory holds the source files for the API documentation,
which is generated from the RIOT source code (header and `doc.md` files) itself.

New guides should not be added directly to the Doxygen documentation,
but rather to the Starlight site, which can then link to the API documentation as needed,
unless the guide is very specific to the API and would not fit well within the Starlight site.
