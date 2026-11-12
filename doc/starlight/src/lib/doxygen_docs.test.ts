/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Tests of the documentation of boards and CPUs as a whole, see
 * `doxygen_docs.ts`. Run with `make test`.
 *
 * The tests work on small source trees in a temporary directory.
 * The heading ids come from the Markdown processor.
 */

import { test } from "node:test";
import assert from "node:assert/strict";
import { promises as fs } from "node:fs";
import os from "node:os";
import path from "node:path";
import {
  convertDoxygenDoc,
  type DoxygenDocFile,
  type DoxygenDocSource,
  findDoxygenDocs,
  loadDoxygenSite,
} from "./doxygen_docs.ts";
import { markdownProcessor } from "./markdown_processor.ts";

const API = "https://api.example.org";

const processor = await markdownProcessor.createRenderer({ syntaxHighlight: false });

/** The heading ids of `markdown`. */
async function renderHeadingIds(markdown: string): Promise<string[]> {
  const { metadata } = await processor.render(markdown);
  return metadata.headings.map((heading) => heading.slug);
}

/**
 * Run `run` with a directory that contains `files` (path → content) and is
 * removed afterwards.
 */
async function withTree(
  files: Record<string, string>,
  run: (root: string) => Promise<void>,
): Promise<void> {
  const root = await fs.mkdtemp(path.join(os.tmpdir(), "doxygen-docs-"));
  try {
    for (const [file, content] of Object.entries(files)) {
      await fs.mkdir(path.dirname(path.join(root, file)), { recursive: true });
      await fs.writeFile(path.join(root, file), content);
    }
    await run(root);
  } finally {
    await fs.rm(root, { recursive: true, force: true });
  }
}

function boards(root: string): DoxygenDocSource {
  return { dir: path.join(root, "boards"), groupPrefix: "boards", routePrefix: "boards" };
}

function cpus(root: string): DoxygenDocSource {
  return { dir: path.join(root, "cpu"), groupPrefix: "cpu", routePrefix: "cpus" };
}

/** The file with the page id `id`. */
function byId(docs: DoxygenDocFile[], id: string): DoxygenDocFile {
  const doc = docs.find((d) => d.id === id);
  assert.ok(doc, `no page "${id}" in ${docs.map((d) => d.id).join(", ")}`);
  return doc;
}

/** A file with a group and a body. */
function group(name: string, title: string, ingroup: string, body = "Text."): string {
  return `@defgroup ${name} ${title}\n@ingroup ${ingroup}\n@brief Support for ${title}.\n\n${body}\n`;
}

test("the files of a source tree are found in a fixed order", async () => {
  await withTree(
    {
      "boards/foo/doc.md": group("boards_foo", "Foo", "boards"),
      "boards/foo/doc_extra.doc.md": group("boards_foo_extra", "Foo Extra", "boards_foo"),
      "boards/foo/Makefile.features": "",
      "boards/bar/doc.md": group("boards_bar", "Bar", "boards"),
      "boards/bar/Makefile.features": "",
      "boards/bar/README.md": "not documentation",
      // a directory of common code
      "boards/common/doc.md": group("boards_common", "Common", "boards"),
      "boards/common/nucleo/doc.md": group("boards_common_nucleo", "Nucleo", "boards"),
      "boards/common/nucleo/Makefile.features": "",
      // a group without a body only gets a page as a board
      "boards/common/empty/doc.md": "@defgroup boards_common_empty Empty\n@ingroup boards\n@brief No body.\n",
      "boards/baz/doc.md": "@defgroup boards_baz Baz\n@ingroup boards\n@brief No body.\n",
      "boards/baz/Makefile.features": "",
      // not documentation of a board
      "boards/vendor/x/doc.md": group("boards_x", "X", "boards"),
      "boards/.hidden/doc.md": group("boards_hidden", "Hidden", "boards"),
      "boards/doc.md": group("boards", "Boards", "config"),
    },
    async (root) => {
      const docs = await findDoxygenDocs(boards(root));
      assert.deepEqual(
        docs.map((doc) => doc.id),
        ["bar", "baz", "common", "common/nucleo", "foo", "foo/doc_extra.doc"],
      );
      // only the `doc.md` of a board is the page of a board
      assert.deepEqual(
        docs.filter((doc) => doc.isModulePage).map((doc) => doc.id),
        ["bar", "baz", "foo"],
      );

      const extra = byId(docs, "foo/doc_extra.doc");
      assert.equal(extra.pageUrl, "/boards/foo/doc_extra.doc/");
      assert.equal(extra.docPath, path.join(root, "boards/foo/doc_extra.doc.md"));
      assert.equal(extra.docName, "foo");
      assert.equal(extra.parsed.title, "Foo Extra");
      assert.equal(byId(docs, "common/nucleo").docName, "nucleo");
    },
  );
});

test("the paths of a file are relative to the root of the repository", async () => {
  await withTree({ "doc/starlight/.keep": "", "boards/foo/doc.md": group("boards_foo", "Foo", "boards") }, async (root) => {
    // the source trees are given relative to `doc/starlight`
    const cwd = process.cwd();
    process.chdir(path.join(root, "doc/starlight"));
    try {
      const [doc] = await findDoxygenDocs({ ...boards(root), dir: "../../boards" });
      assert.equal(doc.docPath, "../../boards/foo/doc.md");
      assert.equal(doc.sourcePath, "boards/foo/doc.md");
      assert.equal(doc.sourceDir, "boards/foo");
    } finally {
      process.chdir(cwd);
    }
  });
});

test("a page links to the pages of all source trees and to the API documentation", async () => {
  const tagFile = `<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.14.0">
  <compound kind="group">
    <name>boards_foo</name>
    <title>Foo</title>
    <filename>group__boards__foo__from__tagfile.html</filename>
  </compound>
  <compound kind="group">
    <name>drivers_periph_gpio</name>
    <title>GPIO</title>
    <filename>group__drivers__periph__gpio.html</filename>
  </compound>
</tagfile>
`;
  await withTree(
    {
      "boards/foo/doc.md": group(
        "boards_foo",
        "Foo Board",
        "boards",
        "See \\ref cpu_bar_flashing, \\ref drivers_periph_gpio and \\ref unknown_thing.",
      ),
      "boards/foo/Makefile.features": "",
      // without a group the title is the name of the directory
      "boards/my-board/doc.md": "Just text.\n",
      "cpu/bar/doc.md": group("cpu_bar", "Bar", "cpu", "# Flashing {#cpu_bar_flashing}\n\nSee \\ref boards_foo."),
      "riot.tag": tagFile,
    },
    async (root) => {
      const sources = [boards(root), cpus(root)];
      const apiDoc = { baseUrl: API, localTagFile: path.join(root, "riot.tag") };

      const site = await loadDoxygenSite(sources, apiDoc, renderHeadingIds);
      const boardDocs = site.docs.get(sources[0])!;
      const foo = convertDoxygenDoc(byId(boardDocs, "foo"), site);
      assert.equal(foo.title, "Foo Board");
      assert.equal(foo.description, "Support for Foo Board.");
      assert.equal(foo.apiUrl, `${API}/group__boards__foo__from__tagfile.html`);
      assert.equal(
        foo.markdown.trim(),
        `See [Flashing](/cpus/bar/#cpu_bar_flashing), [GPIO](${API}/group__drivers__periph__gpio.html) and \`unknown_thing\`.`,
      );
      assert.deepEqual(foo.unresolved, ["unknown_thing"]);

      const myBoard = convertDoxygenDoc(byId(boardDocs, "my-board"), site);
      assert.equal(myBoard.title, "my board");
      assert.equal(myBoard.description, undefined);
      assert.equal(myBoard.apiUrl, `${API}/`);

      const bar = convertDoxygenDoc(byId(site.docs.get(sources[1])!, "bar"), site);
      assert.match(bar.markdown, /See \[Foo Board\]\(\/boards\/foo\/\)\./);

      // Without a tag file the page of a group is derived from its name, and
      // nothing is reported as unresolved.
      const offline = await loadDoxygenSite(sources, { ...apiDoc, tagFile: "" }, renderHeadingIds);
      assert.equal(offline.api, undefined);
      const offlineFoo = convertDoxygenDoc(byId(offline.docs.get(sources[0])!, "foo"), offline);
      assert.equal(offlineFoo.apiUrl, `${API}/group__boards__foo.html`);
      assert.match(offlineFoo.markdown, /\[drivers_periph_gpio\]\(https:\/\/api\.example\.org\/group__drivers__periph__gpio\.html\)/);
      assert.deepEqual(offlineFoo.unresolved, []);
    },
  );
});

test("the ids of the headings come from the Markdown processor", async () => {
  await withTree(
    {
      // `flashing` is also the slug of the first heading
      "cpu/bar/doc.md": group(
        "cpu_bar",
        "Bar",
        "cpu",
        "## Flashing\n\nText.\n\n## Programming {#flashing}\n\nText.\n\n## Pins of \\ref boards_foo {#pins-of-foo-board}",
      ),
      "boards/foo/doc.md": group("boards_foo", "Foo Board", "boards", 'See \\ref flashing and \\ref pins-of-foo-board "the pins".'),
    },
    async (root) => {
      const sources = [boards(root), cpus(root)];
      const site = await loadDoxygenSite(sources, { baseUrl: API, localTagFile: "", tagFile: "" }, renderHeadingIds);

      const bar = byId(site.docs.get(sources[1])!, "bar");
      assert.deepEqual(site.headingIds.get(bar), ["flashing", "programming", "pins-of-foo-board"]);
      const markdown = convertDoxygenDoc(bar, site).markdown;
      assert.match(markdown, /^## <a id="flashing-1"><\/a>Programming$/m);
      // no anchor, the heading has this id already
      assert.match(markdown, /^## Pins of \[Foo Board\]\(\/boards\/foo\/\)$/m);

      const foo = convertDoxygenDoc(byId(site.docs.get(sources[0])!, "foo"), site);
      assert.equal(
        foo.markdown.trim(),
        "See [Programming](/cpus/bar/#flashing-1) and [the pins](/cpus/bar/#pins-of-foo-board).",
      );
    },
  );
});
