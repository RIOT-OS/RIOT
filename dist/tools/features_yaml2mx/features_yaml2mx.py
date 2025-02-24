#!/usr/bin/env python3
"""
Command line utility generate trivial Makefile listing all existing features in
RIOT and a matching documentation in Markdown format from single YAML file.
"""
import argparse
import yaml


def collect_features(parsed):
    """
    Collect all features from a parsed YAML file

    :param parsed: Parsed YAML file
    :type parsed: dict
    :return: list of features in no particular, possible unstable, order
    :rtype: list
    """
    result = []
    for group in parsed.get("groups", []):
        result += collect_features(group)
    for feature in parsed.get("features", []):
        result.append(feature["name"])
    return result


def write_makefile(outfile, yaml_path, parsed):
    """
    Extract the list of features from the given parsed YAML file and writes
    them into file at the given path in Makefile syntax, e.g.

        FEATURES_EXISTING := \
            feat_a \
            feat_b \
            feat_c \
            #

    :param outfile: path to the Makefile to write the features to
    :type outfile: str
    :param yaml_path:   Path to the source YAML file
    :type yaml_path:    str
    :param parsed: the parsed YAML file
    :type parsed: dict
    """
    outfile.write(f"""\
# WARNING: This has been auto-generated from {yaml_path}.
#          Do not edit this by hand, but update {yaml_path} instead.
#          Finally, run `make generate-features` in the root of the RIOT repo.
""")
    outfile.write("FEATURES_EXISTING := \\\n")
    for feature in sorted(collect_features(parsed)):
        outfile.write(f"    {feature} \\\n")
    outfile.write("    #\n")
    outfile.flush()


def write_md_section(outfile, group, level):
    """
    Write a section documenting certain features to the given file in markdown
    format.

    :param outfile: The file to write the section to
    :type outfile:  file
    :param group:   The group content (e.g. a subtree from the parsed YAML)
    :type group:    dict
    :param level:   The current section level (e.g. 1=section, 2=subsection)
    :type level:    int
    """
    title = group.get("title")
    outfile.write("#" * level + f" {title}" if title else "" + "\n")
    if "help" in group:
        outfile.write("\n")
        outfile.write(group["help"])
        outfile.write("\n")

    if "features" in group:
        outfile.write("\n")
        outfile.write("""\
| Feature                           | Description                                                                   |
|:--------------------------------- |:----------------------------------------------------------------------------- |
""")

        for feature in group["features"]:
            name = f"`{feature['name']}`"
            description = feature['help'].strip().replace("\n", " ")
            outfile.write(f"| {name:<33} | {description:<77} |\n")

    for group in group.get('groups', []):
        outfile.write("\n")
        write_md_section(outfile, group, level + 1)


def write_mdfile(outfile, yaml_path, parsed):
    """
    Write the given contents from the parsed YAML file as markdown
    documentation to the given file

    :param outfile:     The file to write the documentation to
    :type outfile:      file
    :param yaml_path:   Path to the source YAML file
    :type yaml_path:    str
    :param parsed:      The parsed YAML file contents
    :type parsed:       dict
    """
    outfile.write(f"""\
# List of Features (Features as Build System Enties)            {{#feature-list}}
<!--
WARNING: This has been auto-generated from {yaml_path}.
         Do not edit this by hand, but update {yaml_path} instead.
         Finally, run `make generate-features` in the root of the RIOT repo.
-->

[TOC]

""")
    write_md_section(outfile, parsed, 0)


def convert_features(yaml_file, mk_file, md_file):
    """
    Convert the YAML file identified by the given path to a Makefile and
    to a markdown file, if their paths are given.

    :param yaml_file:   Path to the YAML file to read
    :type yaml_file:    str
    :param mk_file:     Path to the Makefile to write the features to or None
                        for not writing the Makefile
    :type mk_file:      str or None
    :param md_file:     Path to the markdown file to write the doc to or None
                        for not writing the doc
    :type md_file:      str or None
    """
    with open(yaml_file, 'rb') as file:
        parsed = yaml.safe_load(file)

    if mk_file is not None:
        with open(mk_file, 'w', encoding="utf-8") as file:
            write_makefile(file, yaml_file, parsed)

    if md_file is not None:
        with open(md_file, 'w', encoding="utf-8") as file:
            write_mdfile(file, yaml_file, parsed)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Generate documentation for features in markdown ' +
                        'format and a Makefile listing existing features'
            )
    parser.add_argument('INPUT', type=str, default=None,
                        help="Input file in YAML format")
    parser.add_argument('--output-md', type=str, default=None,
                        help="Output file to write the markdown " +
                             "documentation to (default: no documentation")
    parser.add_argument('--output-makefile', type=str, default=None,
                        help="Output file to write the makefile to " +
                             "(default: no makefile generated)")

    args = parser.parse_args()

    convert_features(args.INPUT, args.output_makefile, args.output_md)
