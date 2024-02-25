#!/usr/bin/env python3
"""Turn extracted RDF data into a board x features table"""

from typing import Optional

import rdflib
from markupsafe import Markup

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


def size(size: Optional[rdflib.Literal]) -> Markup:
    if size is None:
        return "?"
    size = size.toPython()

    suffix = ""

    if size >= 1024 * 1024:
        val = size / 1024 / 1024
        suffix = "MiB"
    elif size >= 1024:
        val = size / 1024
        suffix = "KiB"
    else:
        val = size
        suffix = None

    if val == int(val):
        val = str(int(val))
    else:
        val = "%.1f" % val

    if suffix:
        return Markup("{}\N{NARROW NO-BREAK SPACE}{}").format(val, suffix)
    else:
        return Markup("{}").format(val)


def colspan(cells: list[Markup]) -> list[Markup]:
    last = cells[0]
    repeat = 1
    result = []
    for cell in cells[1:] + [None]:
        if cell == last:
            repeat += 1
        else:
            if repeat == 1:
                result.append(last)
            else:
                result.append(
                        Markup('<td colspan="{}"').format(repeat) +
                        last.removeprefix("<td"))
            repeat = 1
        last = cell
    return result


g = rdflib.Graph()
g.parse("doc/rdf/features.ttl", format="turtle")
g.parse("doc/rdf/info.ttl", format="turtle")
g.parse("doc/rdf/terms.ttl", format="turtle")
g.parse("doc/rdf/doxygen.ttl", format="turtle")

boards = g.query("""
        SELECT
            ?cpuname ?cpu ?cpudoc ?boardname ?board ?boarddoc ?ram ?rom
            ?wordsizelabel ?architecture ?cpufeature
        WHERE {
            ?board terms:cpu ?cpu .
            ?cpu rdfs:label ?cpuname ; terms:doc ?cpudoc .
            ?board terms:doc ?boarddoc ; rdfs:label ?boardname .
            OPTIONAL {
                ?board terms:ram ?ram ; terms:rom ?rom .
            }
            # could have these for every functional group
            OPTIONAL {
                ?board terms:providesFeature ?wordsize .
                ?wordsize rdfs:label ?wordsizelabel ;
                   dct:subject / skos:prefLabel "Word size"@en .
            }
            OPTIONAL {
                ?board terms:providesFeature ?architecture .
                ?architecture dct:subject /
                    skos:prefLabel "Architecture grouping"@en .
            }
            OPTIONAL {
                ?board terms:providesFeature ?cpufeature .
                ?cpufeature dct:subject /
                    skos:broader* /
                    skos:prefLabel "CPU Grouping"@en .
                # Ensure that we're getting the most specific CPU feature and
                # not just some "ARM Cortex" feature.
                FILTER NOT EXISTS {
                    ?cpufeature dct:subject ?broadgroup .
                    ?board terms:providesFeature ?deepercpufeature .
                    ?deepercpufeature dct:subject/skos:broader+ ?broadgroup .
                }
            }
        }
        GROUP BY ?board
        ORDER BY ?wordsizelabel ?architecture ?cpuname ?cpufeature ?boardname
        """, initNs=ns)

features = g.query("""
        SELECT
            ?feature ?group_1_label ?group_2_label ?group_3_label
        WHERE {
            ?feature a terms:Feature .

            # Might make them optional, but every feature is in a group by
            # construction.
            ?feature dct:subject ?group_last .
            ?group_1 skos:topConceptOf ?scheme ;
                skos:prefLabel ?group_1_label ;
                skos:narrower* ?group_last .

            OPTIONAL {
                ?group_1 skos:narrower ?group_2 .
                ?group_2 skos:prefLabel ?group_2_label ;
                    skos:narrower* ?group_last .
            }

            OPTIONAL {
                ?group_1 skos:narrower ?group_2 .
                ?group_2 skos:narrower ?group_3 .
                ?group_3 skos:prefLabel ?group_3_label ;
                    skos:narrower* ?group_last .
            }

            # These just represent what we already have explicitly as the CPU
            # data
            FILTER NOT EXISTS {
                ?feature dct:subject /
                    skos:broader* /
                    skos:prefLabel "CPU Grouping"@en .
            }
            # Both its subcategories were used as functional groups
            FILTER NOT EXISTS {
                ?feature dct:subject /
                    skos:broader* /
                    skos:prefLabel "Architecture Features"@en .
            }
        }
        # Don't need to group as long as the hierarchy is a tree (and if it
        # becomes more complex, the group parts of the query cease to make
        # sense)
        ORDER BY ?group_1 ?group_2 ?group_3 ?feature
        """, initNs=ns)


def featurelabel(node) -> Markup | str:
    # Querying at render time is probably less efficient than having all
    # possible relations resolved at query time, but maybe not -- what matters
    # more is that the queries are more readable if the details are not
    # resolved in there, and that having the function makes it easier to create
    # consistent visuals.
    label, = g.objects(node, rdflib.RDFS.label)
    comments = list(g.objects(node, rdflib.RDFS.comment))
    href, = g.objects(node, ns["terms"]["doc"])
    if comments:
        return Markup('<a href="{}" title="{}">{}</a>').format(
                href,
                comments[0],
                label
                )
    else:
        return Markup('<a href="{}">{}</a>').format(href, comments[0], label)


rows = []
for b in boards:
    cells = [
            Markup('<td style="text-align: right; vertical-align:top">'
                   '<span style="position:sticky; top:0">{}</span></td>'
                   ).format(b.wordsizelabel.removeprefix("arch_")),
            Markup('<td style="text-align: right; vertical-align:top">'
                   '<span style="position:sticky; top:0">{}</span></td>'
                   ).format(featurelabel(b.architecture)),
            Markup('<td style="text-align: left; vertical-align:top">'
                   '<a href="{}" style="position:sticky; top:0">{}</a></td>'
                   ).format(b.cpudoc, b.cpuname),
            Markup('<td style="text-align: left; vertical-align:top">'
                   '<span style="position:sticky; top:0">{}</span></td>'
                   ).format(featurelabel(b.cpufeature)),
            # not wrapping -- some may rows will still be multiline because the
            # CPU wraps and has more lines than boards, but most will be 1 high
            Markup('<td style="text-align: left; white-space: pre">'
                   '<a href="{}">{}</a></td>'
                   ).format(b.boarddoc, b.boardname),
            Markup('<td style="text-align: right">{}</td>'
                   ).format(size(b.ram)),
            Markup('<td style="text-align: right">{}</td>'
                   ).format(size(b.rom)),
            ]
    for f in features:
        presence = (b.board, ns['terms']['providesFeature'], f.feature) in g
        color = "green" if presence else "red"
        cells.append(Markup('<td style="background-color: {}">{}</td>'
                            ).format(color, "Y" if presence else "N"))
    rows.append(cells)

# Place colspans
GROUPCOLS = 6
# applying this to features in general will decrease readability, thus limiting
# to the first 6
prevcells = [None] * GROUPCOLS
prevcell_ranges = [(0, 0)] * GROUPCOLS
for (row, cells) in enumerate(rows + [[None] * GROUPCOLS]):
    next_ranges = []
    for (col, (start, count)) in enumerate(prevcell_ranges):
        if cells[col] == prevcells[col]:
            next_ranges.append((start, count + 1))
        else:
            if count > 1:
                rows[start][col] = Markup(f'<td rowspan="{count}"') \
                        + rows[start][col].removeprefix("<td")
                for removed in range(1, count):
                    rows[start + removed][col] = ""
            next_ranges.append((row, 1))
    prevcell_ranges = next_ranges
    prevcells[:GROUPCOLS] = cells

groupcell = Markup("<td>{}</td>")
group1 = [groupcell.format(f.group_1_label or "") for f in features]
group2 = [groupcell.format(f.group_2_label or "") for f in features]
group3 = [groupcell.format(f.group_3_label or "") for f in features]

group1 = colspan(group1)
group2 = colspan(group2)
group3 = colspan(group3)

features = Markup("").join(Markup(
    '<th style="vertical-align: bottom">'
    '<span style="'
    'writing-mode: vertical-lr; transform: rotate(180deg); min-width: 1em">'
    '{}</span></th>').format(featurelabel(f.feature)) for f in features)
rows = Markup("\n").join(
        Markup("<tr>{}</tr>").format(Markup("").join(r)) for r in rows
        )

template = Markup("""
<!DOCTYPE html>
<html>
<head>
<title>RIOT boards and features</title>
<meta charset="utf-8" />
</head>
<table border="1">
<tr>
    <th colspan="7" rowspan="3"></th>
    {}
</tr>
<tr>
    {}
</tr>
<tr>
    {}
</tr>
<tr style="vertical-align: bottom">
    <th><abbr title="Word size">Words.</abbr></th>
    <th><abbr title="Architecture">Arch.</abbr></th>
    <th>CPU<sup title="as per the CPU variable">*</sup></th>
    <th>CPU<sup title="as per features">*</sup></th>
    <th>Board</th>
    <th>RAM</th>
    <th>ROM</th>
    {}
</tr>
{}
</table>
</html>
""").format(
        Markup("").join(group1),
        Markup("").join(group2),
        Markup("").join(group3),
        features,
        rows,
        )

print(template)
