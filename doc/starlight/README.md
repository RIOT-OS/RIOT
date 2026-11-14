# RIOT Guide Site

This directory holds the [Starlight](https://starlight.astro.build/) sources
of the [guide site](https://guide.riot-os.org/) of RIOT.
[Creating Guides & How Starlight Works](https://guide.riot-os.org/misc/how_to_doc/)
explains how to write a guide and how to test it locally.

## Doxygen tag file

The build uses a Doxygen tag file to link the board and CPU pages to the
[API documentation](https://api.riot-os.org/).
It takes the first of these:

1. The path or URL in `RIOT_DOXYGEN_TAGFILE`.
2. `doc/doxygen/html/riot.tag` from a local `make doc`.
3. A download of `https://api.riot-os.org/riot.tag`.

Case 3 needs network access, and the links follow the published API
documentation instead of your branch.
To build without the tag file, set the variable to an empty value:

```sh
RIOT_DOXYGEN_TAGFILE= make build
```

The build then shows API references as code without links.
