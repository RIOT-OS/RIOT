#!/usr/bin/env python3
"""
Command line utility generate trivial Makefile listing all existing features in
RIOT and a matching documentation in Markdown format from single YAML file.
"""
import sys
import argparse
import yaml
import rdflib
from typing import Optional
import pycddl
import zcbor
from pathlib import Path
import cbor2

# We may offer building with a different ns_main later to compare statements
# about different versions of RIOT in a single graph, but for the time being,
# the assumption that what we are working with is the main branch is good
# enough.
ns_main = "https://ns.riot-os.org/by-version/main/"
ns = dict(
        feature=rdflib.Namespace(ns_main + "feature/"),
        featureskos=rdflib.Namespace(ns_main + "feature-skos/"),
        terms=rdflib.Namespace(ns_main + "terms/"),

        skos=rdflib.Namespace('http://www.w3.org/2004/02/skos/core#'),
        dct=rdflib.Namespace('http://purl.org/dc/terms/'),
        )

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
| Feature                                                                             | Description                                                                   |
|:----------------------------------------------------------------------------------- |:----------------------------------------------------------------------------- |
""")

        for feature in group["features"]:
            name = f"`{feature['name']}`"
            description = feature['help'].strip().replace("\n", " ")
            outfile.write(f"| {name:<33} @anchor feature_{feature['name']:<33} | {description:<77} |\n")

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

def populate_graph(g: rdflib.Graph, skosscheme: rdflib.term.Node, parentnode: Optional[rdflib.term.Node], content: dict):
    help = content.get("help", None)
    title = content["title"]

    thisnode = rdflib.BNode()
    # Not making everything perfectly short-identifier-safe because it is not
    # critical: the serialization takes care of falling back to the universal
    # URI form eg. if there is a ² in there.
    thisnode = ns['featureskos'][title
                .replace(' ', '_')
                .replace('/', '_')
                # https://wileylabs.github.io/askos/ trips over those when parsing Turtle
                .replace('(', '_')
                .replace(')', '_')
                ]
    if (thisnode, None, None) in g:
        raise RuntimeError(f"Tree contains duplicate category {title}. Ensure "
                "unique names or alter the pattern in which featureskos: names "
                "are generated")
    if parentnode:
        g.add((parentnode, ns['skos']['narrower'], thisnode))
        # One can argue either way about whether we should include this statement
        # that obviously follows from the line above;
        # https://skos-play.sparna.fr/skos-testing-tool/ thinks we should. Same
        # goes for `topConceptOf` below.
        g.add((thisnode, ns['skos']['broader'], parentnode))
    else:
        g.add((skosscheme, ns['skos']['hasTopConcept'], thisnode))
        # see comment about "broader" above
        g.add((thisnode, ns['skos']['topConceptOf'], skosscheme))
        g.add((thisnode, rdflib.RDF.type, ns['skos']['Concept']))
    g.add((thisnode, rdflib.RDF.type, ns['skos']['Concept']))
    g.add((thisnode, ns['skos']['inScheme'], skosscheme))
    g.add((thisnode, ns['skos']['prefLabel'], rdflib.Literal(title, lang="en")))
    if help:
        g.add((thisnode, ns['skos']['definition'], rdflib.Literal(help, lang="en")))

    for feature in content.get("features", []):
        name = feature["name"]
        help = feature["help"]
        g.add((ns['feature'][name], rdflib.RDFS.label, rdflib.Literal(name, lang="en")))
        g.add((ns['feature'][name], rdflib.RDFS.comment, rdflib.Literal(help, lang="en")))
        g.add((ns['feature'][name], ns['terms']['doc'], rdflib.URIRef(f"https://doc.riot-os.org/md_doc_2doxygen_2src_2feature__list.html#feature_{name}")))
        g.add((ns['feature'][name], rdflib.RDF.type, ns['terms']['Feature']))
        g.add((ns['feature'][name], ns['dct'].subject, thisnode))

    for group in content.get("groups", []):
        populate_graph(g, skosscheme, thisnode, group)

def write_ttl(outfile, parsed):
    g = rdflib.Graph()
    for (k, v) in ns.items():
        g.bind(k, v)

    skosscheme = ns['featureskos']['scheme']
    g.add((skosscheme, rdflib.RDF.type, ns['skos']['ConceptScheme']))
    g.add((skosscheme, ns['skos']['prefLabel'], rdflib.Literal("RIOT OS feature hierarchy", lang="en")))
    for content in parsed['groups']:
        populate_graph(g, skosscheme, None, content)

    outfile.write(b"# This file is generated by `make doc`\n\n")
    g.serialize(outfile, format="turtle", encoding="utf-8")

def convert_features(yaml_file, mk_file, md_file, ttl_file: Optional[str]):
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
    :param md_file:     Path to the RDF N-triples file to write the doc to or
                        None for not writing the doc
    """
    with open(yaml_file, 'rb') as file:
        parsed = yaml.safe_load(file)

    # PyCDDL is not perfect because it doesn't catch rogue elements: https://github.com/anweiss/cddl/issues/221
    schema = pycddl.Schema(open(Path(__file__).parent / "schema.cddl").read())
    schema.validate_cbor(cbor2.dumps(parsed))

    # zcbor is not perfect either because it doesn't process the .within (and
    # its error output just contains the whole input)
    schema = zcbor.DataTranslator.from_cddl(
            open(Path(__file__).parent / "schema-simplified.cddl").read(),
            default_max_qty=sys.maxsize
            )
    schema.my_types['root'].from_yaml(open(yaml_file).read())

    if mk_file is not None:
        with open(mk_file, 'w', encoding="utf-8") as file:
            write_makefile(file, yaml_file, parsed)

    if md_file is not None:
        with open(md_file, 'w', encoding="utf-8") as file:
            write_mdfile(file, yaml_file, parsed)

    if ttl_file is not None:
        with open(ttl_file, 'wb') as file:
            write_ttl(file, parsed)


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
    parser.add_argument('--output-ttl', type=str, default=None,
                        help="Output file to write RDF Turtle to " +
                            "(default: none generated)")

    args = parser.parse_args()

    convert_features(args.INPUT, args.output_makefile, args.output_md, args.output_ttl)
