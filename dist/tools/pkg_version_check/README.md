# pkg version check script

This script checks for outdated packages in the `pkg` directory.

Required Python modules: *requests*.

The script will get the `PKG_VERSION` and `PKG_URL` in a package Makefile. Then it will get the latest release hash of the package on GitHub and compare that with the `PKG_VERSION` hash. If it cannot find any release for the package on GitHub, it will fallback to getting the latest commit hash and compare that with the `PKG_VERSION` hash.

Currently, the script can only check packages that are hosted on GitHub.

The script will check every packages inside the `pkg` directory, then output the total number of outdated packages, the name of the outdated packages, the number of packages the script cannot check, and the name of the packages the script cannot check.

A GitHub API token is required to retrieve information from Github. The script will ask for the API token upon execution.
