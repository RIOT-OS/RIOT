Tool to export features described in a YAML to formats usable by our build system
=================================================================================

The python script in this folder converts features specified in a YAML file to
a Makefile for use to check provided and requested features against, as well
as a markdown file for consumption by Doxygen to document the features.

The YAML file containing the features is `features.yaml` in the root of the
repo. Its syntax is documented in a comment on top of said file.

This script should be invoked by running `make generate-features` in the root
of the repository.
