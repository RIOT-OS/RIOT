# SBOM Creation for RIOT OS Based Applications

## About

The `sbom_riot` Python package is designed to generate and manage the Software
Bill of Materials (SBOM) for RIOT OS based software projects.

## Installing

Please build & install using [poetry](https://python-poetry.org/docs/).
Executing `poetry build && poetry install` should install the package and its
dependencies in your current virtual environment.
Distribution through pip is currently not planned.

## Running

This package is intended to be mainly executed through its command line
interface. After activating your virtual environment you should have
the `riot-sbom` script installed there.

Traced builds may take a longer time. The package therefore allows to save
data in between steps. Running a traced build on an application can be achieved
with:

```console
riot-sbom --app-dir <path-to-your-application> --save-app-info <path/to/app_info.pkl>
```

Saving is in pickle format. It is not intended for storage of data.
If the save option is provided, updates to the application information
will be saved / overwritten after the build and after each plugin execution.
Loading from a file can be combined with saving after executing plugins.



With a valid application information object, plugins can be executed to
aggregate information on the packages and files of the build or write
output to files:

```console
riot-sbom --load-app-info <path/to/app_info.pkl> \
    --output-file-prefix <path/to/outfilebase> \
    --plugin-pipeline copyrights-scanner authors-scanner spdx-identifiers-scanner \
                      system-package-provider infer-file-data-from-package spdx-generator
```

Available default plugins can be listed via `riot-sbom --list-plugins`.

All tasks can be executed in one go of course, without saving to the file system:

```console
riot-sbom --app-dir <path-to-your-application> \
    --output-file-prefix <path/to/outfilebase> \
    --plugin-pipeline copyrights-scanner authors-scanner spdx-identifiers-scanner \
                      system-package-provider infer-file-data-from-package spdx-generator
```

## Extending

The package supports dynamic loading of plugins.
If you have plugins implementing `riot_sbom.processing.plugin_type.Plugin`,
you can provide their directories on the command line for loading.

The following command will load and list all available plugins:

```console
riot-sbom --external-plugin-dirs <path/to/plugin/dir1> <path/to/plugin/dir2> --list-plugins
```
