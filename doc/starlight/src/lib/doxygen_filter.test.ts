/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Tests of the conversion of Doxygen Markdown, see `doxygen_filter.ts`.
 * Run with `make test`.
 *
 * The tests check the Markdown that the converter emits for small inputs.
 * They look for fragments of the output instead of comparing whole
 * documents, so that they don't depend on blank lines.
 * The heading ids come from the Markdown processor.
 */

import { describe, test } from "node:test";
import assert from "node:assert/strict";
import {
  AnchorIndex,
  collectDoxygenAnchors,
  headingMarkdown,
  parseDoxygenDoc,
  transformDoxygenMarkdown,
  type DoxygenDoc,
  type TransformContext,
} from "./doxygen_filter.ts";
import { parseDoxygenTagFile } from "./doxygen_tagfile.ts";
import { markdownProcessor } from "./markdown_processor.ts";

/* -------------------------------------------------------------------------- */
/* Fixtures                                                                   */
/* -------------------------------------------------------------------------- */

const API = "https://api.example.org";
const PAGE_URL = "/boards/foo/";

const GPIO_PAGE = `${API}/group__drivers__periph__gpio.html`;
const GPIO_INIT = `${GPIO_PAGE}#ga_init`;
const GPIO_READ = `${GPIO_PAGE}#ga_read`;
const GPIO_WRITE = `${GPIO_PAGE}#ga_write`;
const GPIO_T = `${GPIO_PAGE}#ga_t`;
const GPIO_PIN = `${GPIO_PAGE}#ga_pin`;
const PHYDAT_MIN = `${API}/group__sys__phydat.html#ga_min`;
const PHYDAT_MAX = `${API}/group__sys__phydat.html#ga_max`;
const EXPECT_EQ = `${API}/group__unittests.html#ga_eq`;
const LL_DIVIDER = `${API}/group__cpu__stm32.html#ga_div`;
const UNICOAP_DRIVERS = `${API}/group__net__unicoap__drivers.html`;

/** A tiny tag file in Doxygen's one-element-per-line layout. */
const TAGFILE = `<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>gpio.h</name>
    <path>drivers/include/periph/</path>
    <filename>periph_2gpio_8h.html</filename>
    <member kind="function">
      <type>int</type>
      <name>gpio_init</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>ga_init</anchor>
      <arglist>(gpio_t pin, gpio_mode_t mode)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>gpio_read</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>ga_read</anchor>
      <arglist>(gpio_t pin)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>gpio_write</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>ga_write</anchor>
      <arglist>(gpio_t pin, bool value)</arglist>
    </member>
    <member kind="typedef">
      <type>unsigned int</type>
      <name>gpio_t</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>ga_t</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>GPIO_PIN</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>ga_pin</anchor>
      <arglist>(x, y)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>phydat.h</name>
    <path>sys/include/</path>
    <filename>phydat_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PHYDAT_MIN</name>
      <anchorfile>group__sys__phydat.html</anchorfile>
      <anchor>ga_min</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PHYDAT_MAX</name>
      <anchorfile>group__sys__phydat.html</anchorfile>
      <anchor>ga_max</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>embUnit.h</name>
    <path>sys/include/embUnit/</path>
    <filename>embUnit_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>EXPECT_EQ</name>
      <anchorfile>group__unittests.html</anchorfile>
      <anchor>ga_eq</anchor>
      <arglist>(expected, actual, msg)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ll_rcc.h</name>
    <path>cpu/stm32/include/</path>
    <filename>ll__rcc_8h.html</filename>
    <member kind="function">
      <type>uint32_t</type>
      <name>LL_Divider</name>
      <anchorfile>group__cpu__stm32.html</anchorfile>
      <anchor>ga_div</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>drivers_periph_gpio</name>
    <title>GPIO</title>
    <filename>group__drivers__periph__gpio.html</filename>
  </compound>
  <compound kind="group">
    <name>net_unicoap_drivers</name>
    <title>Drivers</title>
    <filename>group__net__unicoap__drivers.html</filename>
  </compound>
</tagfile>
`;

const API_INDEX = parseDoxygenTagFile(TAGFILE);

const processor = await markdownProcessor.createRenderer({ syntaxHighlight: false });

/** The heading ids of `markdown`. */
async function renderHeadingIds(markdown: string): Promise<string[]> {
  const { metadata } = await processor.render(markdown);
  return metadata.headings.map((heading) => heading.slug);
}

interface PageFixture {
  content: string;
  pageUrl: string;
}

/** Other board pages of the site, always in the anchor index. */
const OTHER_PAGES: PageFixture[] = [
  {
    pageUrl: "/boards/arduino-due/",
    content: "@defgroup boards_arduino-due Arduino Due\n@ingroup boards\n@brief Support for the Arduino Due.\n\nText.\n",
  },
  {
    pageUrl: "/boards/samr21-xpro/",
    content:
      "@defgroup boards_samr21-xpro Atmel SAM R21 Xplained Pro\n@ingroup boards\n@brief Support for the SAM R21 Xplained Pro.\n\nText.\n",
  },
  {
    pageUrl: "/boards/other/",
    content:
      "@defgroup boards_other Other Board\n@ingroup boards\n@brief Support for the other board.\n\n" +
      "@section boards_other_title Other Board\n\nIntro.\n\n## Flashing {#boards_other_flashing}\n\nText.\n",
  },
];

interface ConvertOptions {
  /** URL of the page under test, default `/boards/foo/` */
  pageUrl?: string;
  /** Further pages whose anchors are indexed */
  others?: PageFixture[];
  /** Leave out the standard `OTHER_PAGES` */
  isolated?: boolean;
  /** Set to `false` to transform without a tag file */
  api?: boolean;
  /** Collects the ids reported as unresolved */
  unresolved?: string[];
}

/** Parse a file of the board documentation. */
function parse(input: string): DoxygenDoc {
  return parseDoxygenDoc(input, "boards");
}

/** The context of a page, as `loadDoxygenSite()` makes it. */
async function makeCtx(input: string, options: ConvertOptions = {}): Promise<TransformContext> {
  const pageUrl = options.pageUrl ?? PAGE_URL;
  // the page under test is the last one
  const pages = [
    ...(options.isolated ? [] : OTHER_PAGES),
    ...(options.others ?? []),
    { content: input, pageUrl },
  ];
  const site = { api: options.api === false ? undefined : API_INDEX, apiBaseUrl: API };
  const sourcePath = (url: string) => `${url.replace(/^\//, "")}doc.md`;
  const draft = new AnchorIndex();
  for (const page of pages) {
    collectDoxygenAnchors(parse(page.content), page.pageUrl, draft);
  }
  const anchors = new AnchorIndex();
  let headingIds: string[] = [];
  for (const page of pages) {
    const ctx = { ...site, pageUrl: page.pageUrl, sourcePath: sourcePath(page.pageUrl), anchors: draft };
    headingIds = await renderHeadingIds(headingMarkdown(parse(page.content), ctx));
    collectDoxygenAnchors(parse(page.content), page.pageUrl, anchors, headingIds);
  }
  return {
    ...site,
    pageUrl,
    sourcePath: sourcePath(pageUrl),
    anchors,
    headingIds,
    onUnresolved: (id: string) => {
      options.unresolved?.push(id);
    },
  };
}

async function convert(input: string, options: ConvertOptions = {}): Promise<string> {
  return transformDoxygenMarkdown(parse(input), await makeCtx(input, options));
}

/* -------------------------------------------------------------------------- */
/* Assertion helpers                                                          */
/* -------------------------------------------------------------------------- */

function assertIncludes(actual: string, expected: string): void {
  if (!actual.includes(expected)) {
    assert.fail(`expected the output to contain:\n${expected}\n--- actual output ---\n${actual}`);
  }
}

function assertExcludes(actual: string, unexpected: string): void {
  if (actual.includes(unexpected)) {
    assert.fail(`expected the output NOT to contain:\n${unexpected}\n--- actual output ---\n${actual}`);
  }
}

/** The fragments occur in this order. */
function assertOrder(actual: string, ...fragments: string[]): void {
  let from = 0;
  for (const fragment of fragments) {
    const at = actual.indexOf(fragment, from);
    if (at < 0) {
      assert.fail(
        `expected ${JSON.stringify(fragment)} after offset ${from}, in order ${JSON.stringify(fragments)}\n--- actual output ---\n${actual}`,
      );
    }
    from = at + fragment.length;
  }
}

/** Blank line runs collapsed, ends trimmed. */
function normalize(md: string): string {
  return md.replace(/[ \t]+$/gm, "").replace(/\n{3,}/g, "\n\n").trim();
}

interface Aside {
  type: string;
  title?: string;
  body: string;
}

/** The Starlight asides of a document, `:::type[title]` … `:::`. */
function asides(md: string): Aside[] {
  const re = /^([ \t]*):::(\w+)(?:\[([^\]\n]*)\])?[ \t]*\n([\s\S]*?)\n\1:::[ \t]*$/gm;
  return [...md.matchAll(re)].map((m) => ({ type: m[2], title: m[3], body: m[4].trim() }));
}

/** Split a document into prose and the contents of its fenced code blocks. */
function splitFences(md: string): { prose: string; code: string[] } {
  const prose: string[] = [];
  const code: string[] = [];
  let fence: string | null = null;
  let buffer: string[] = [];
  for (const line of md.split("\n")) {
    if (fence === null) {
      const open = line.match(/^\s*(`{3,}|~{3,})(.*)$/);
      if (open && !(open[1][0] === "`" && open[2].includes("`"))) {
        fence = open[1];
        buffer = [];
        continue;
      }
      prose.push(line);
    } else {
      const close = line.match(/^\s*(`{3,}|~{3,})\s*$/);
      if (close && close[1][0] === fence[0] && close[1].length >= fence.length) {
        code.push(buffer.join("\n"));
        fence = null;
        continue;
      }
      buffer.push(line);
    }
  }
  if (fence !== null) {
    code.push(buffer.join("\n"));
  }
  return { prose: prose.join("\n"), code };
}

/** All Markdown link targets `[…](url)`, one bracket level deep. */
function linkTargets(md: string): string[] {
  return [...md.matchAll(/\[(?:[^[\]]|\[[^[\]]*\])*\]\(([^)\s]+)\)/g)].map((m) => m[1]);
}

/** Run table-driven cases and report every mismatch at once. */
async function assertCases(
  cases: [input: string, expected: string][],
  run: (input: string) => Promise<string>,
): Promise<void> {
  const mismatches: { input: string; expected: string; actual: string }[] = [];
  for (const [input, expected] of cases) {
    const actual = await run(input);
    if (actual !== expected) {
      mismatches.push({ input, expected, actual });
    }
  }
  assert.deepEqual(mismatches, []);
}

/* -------------------------------------------------------------------------- */
/* Group metadata                                                             */
/* -------------------------------------------------------------------------- */

describe("group metadata", () => {
  test("an undecorated doc.md group yields title, parent and brief, and a clean body", async () => {
    const input =
      "@defgroup boards_stdio STDIO abstraction\n@ingroup sys\n\n@brief STDIO for RIOT\n\n# Abstract\n\nBody text.";
    assert.equal(parse(input).title, "STDIO abstraction");
    assert.deepEqual(parse(input).groups, ["boards_stdio"]);
    assert.deepEqual(parse(input).ingroups, ["sys"]);
    assert.match(parse(input).brief ?? "", /^STDIO for RIOT\.?$/);
    const out = await convert(input);
    assert.match(out, /^## Abstract$/m);
    assertIncludes(out, "Body text.");
    for (const leaked of ["@defgroup", "@ingroup", "@brief", "STDIO for RIOT"]) {
      assertExcludes(out, leaked);
    }
  });

  test("a child group nests under a group defined in another doc.md and links to it", async () => {
    const parent: PageFixture = {
      pageUrl: "/boards/stdio/",
      content: "@defgroup boards_stdio STDIO abstraction\n@ingroup sys\n\n@brief STDIO for RIOT\n\n# Abstract\n\nMore prose.\n",
    };
    const child =
      "@defgroup boards_stdio_null STDIO null driver\n@ingroup boards_stdio\n@brief Dummy stdio.\n\nSee \\ref boards_stdio.\n";
    assert.deepEqual(parse(child).ingroups, ["boards_stdio"]);
    assert.equal(parse(child).title, "STDIO null driver");
    const out = await convert(child, { pageUrl: "/boards/stdio_null/", others: [parent] });
    assertIncludes(out, "See [STDIO abstraction](/boards/stdio/).");
  });

  test("@defgroup, @ingroup, @brief and @{ … @} are metadata, not body", async () => {
    const input = "@defgroup boards_periph_gpio GPIO\n@ingroup periph\n@brief Pins.\n@{\n\nBody.\n\n@}\n";
    assert.equal(parse(input).title, "GPIO");
    assert.deepEqual(parse(input).ingroups, ["periph"]);
    assert.equal(parse(input).brief, "Pins.");
    assert.equal((await convert(input)).trim(), "Body.");
  });

  test("@defgroup takes the first word as id and the rest of the line as title", () => {
    const input = "@defgroup boards_grp My Title goes here\n";
    assert.deepEqual(parse(input).groups, ["boards_grp"]);
    assert.equal(parse(input).title, "My Title goes here");
  });

  test("the group markers @{ and @} are dropped from the body", async () => {
    const out = await convert("Before.\n\n@{\n\nInside.\n\n@}\n\nAfter.");
    assertExcludes(out, "@{");
    assertExcludes(out, "@}");
    assertOrder(out, "Before.", "Inside.", "After.");
  });

  test("@addtogroup, with and without a title, is not rendered", async () => {
    for (const input of [
      "@addtogroup boards_stdio STDIO abstraction\n@{\n\nBody.\n\n@}",
      "@addtogroup boards_stdio\n@{\n\nBody.\n\n@}",
    ]) {
      assert.equal((await convert(input)).trim(), "Body.", input);
    }
  });

  test("a @name line written with @ref does not leak commands into the page", async () => {
    const out = await convert("Intro.\n\n@name @ref EWMA parameters\n@{\n\nText.\n\n@}");
    assertExcludes(out, "@name");
    assertExcludes(out, "@ref");
    assertOrder(out, "Intro.", "Text.");
  });

  test("a @defgroup title written with \\ref keeps the words and loses the command", () => {
    const input = "@defgroup boards_pktq Send queue for \\ref net_gnrc_netif\n@{\n";
    assert.equal(parse(input).title, "Send queue for net_gnrc_netif");
  });

  test("each @defgroup binds its own @ingroup and @brief", async () => {
    const input =
      "@defgroup stdio_usb_serial_jtag USB Serial/JTAG\n@ingroup sys_stdio\n@brief Foreign brief.\n\nForeign body.\n\n" +
      "@defgroup boards_x X board\n@ingroup boards_esp32\n@brief Own brief.\n\nOwn body.\n";
    assert.deepEqual(parse(input).groups, ["boards_x"]);
    assert.deepEqual(parse(input).ingroups, ["boards_esp32"]);
    assert.equal(parse(input).brief, "Own brief.");
    assert.equal(parse(input).title, "X board");
    const out = await convert(input);
    assertIncludes(out, "Own body.");
    assertExcludes(out, "Foreign");
  });

  test("an @ingroup written before the @defgroup binds to it", async () => {
    const input = "@ingroup boards_parent\n@defgroup boards_child Child\n@brief B\n";
    assert.deepEqual(parse(input).ingroups, ["boards_parent"]);
    assertExcludes(await convert(input), "@ingroup");
  });

  test("a wrapped @brief does not swallow the following @ingroup", () => {
    const input = "@defgroup boards_g Group\n@brief A brief that wraps\n          onto a second line\n@ingroup parent\n@{\n";
    assert.equal(parse(input).brief, "A brief that wraps onto a second line");
    assert.deepEqual(parse(input).ingroups, ["parent"]);
  });

  test("@brief continuation lines join with one space", () => {
    const input =
      "@defgroup boards_g G\n@brief Due to RAM restrictions, we need to limit the default GNRC packet\n" +
      "       buffer size on these CPUs.\n";
    assert.equal(
      parse(input).brief,
      "Due to RAM restrictions, we need to limit the default GNRC packet buffer size on these CPUs.",
    );
  });

  test("a heading-led body without @brief stays whole and has no brief", async () => {
    for (const input of [
      "@defgroup boards_g Title\n\n# Overview\nProse describing the module.\n\nMore prose.",
      "@defgroup boards_g Title\n\n# Overview\n\nProse describing the module.\n\nMore prose.",
    ]) {
      assert.equal(parse(input).brief, undefined, input);
      const out = await convert(input);
      assert.match(out, /^## Overview\n/, input);
      assertOrder(out, "Prose describing the module.", "More prose.");
    }
  });

  test("the backslash form \\brief works like @brief", async () => {
    const input = "\\defgroup boards_b Backslash board\n\\brief Short.\n\nBody.";
    assert.equal(parse(input).brief, "Short.");
    assert.equal(parse(input).title, "Backslash board");
    assert.equal((await convert(input)).trim(), "Body.");
  });

  test("a @brief continues on a line that starts with an inline command", async () => {
    const input = "@defgroup boards_g G\n@brief Support for\n\\ref cpu_bar based boards\n@ingroup boards\n\nBody.\n";
    const doc = parse(input);
    // the description is plain text
    assert.equal(doc.brief, "Support for cpu_bar based boards");
    assert.deepEqual(doc.ingroups, ["boards"]);
    assert.equal((await convert(input)).trim(), "Body.");
  });
});

/* -------------------------------------------------------------------------- */
/* HTML comments                                                              */
/* -------------------------------------------------------------------------- */

describe("HTML comments", () => {
  test("commands inside an HTML comment are not extracted, and nothing after it is hidden", async () => {
    const input =
      "@defgroup boards_g Group\n@brief B\n\nProse.\n<!--\n@see @ref UnfinishedThing\nMore commented text.\n-->\n" +
      "@see @ref boards_other\n\nAfter.";
    const out = await convert(input);
    for (const hidden of ["UnfinishedThing", "More commented text", "<!--", "-->"]) {
      assertExcludes(out, hidden);
    }
    const see = asides(out).filter((a) => a.title === "See also");
    assert.equal(see.length, 1, out);
    assert.equal(see[0].body, "[Other Board](/boards/other/)");
    assertOrder(out, "Prose.", ":::note[See also]", "After.");
  });

  test("a comment with a code block in it is removed as a whole", async () => {
    const input =
      "@defgroup boards_g Group\n\nBefore.\n\n<!--\n# Old {#old_section}\n```sh\nmake old\n```\n" +
      "@defgroup boards_old Old\nSee \\ref gone.\n-->\n\nAfter.";
    const unresolved: string[] = [];
    assert.equal(normalize(await convert(input, { unresolved })), "Before.\n\nAfter.");
    assert.deepEqual(unresolved, []);
    assert.deepEqual(parse(input).groups, ["boards_g"]);
    const anchors = new AnchorIndex();
    collectDoxygenAnchors(parse(input), PAGE_URL, anchors);
    assert.equal(anchors.lookup("old_section"), undefined);
    assert.equal(anchors.lookup("boards_old"), undefined);
  });

  test("a comment in a code block is code, and <!-- without --> is text", async () => {
    const out = await convert("```html\n<!-- kept -->\n```\n\nAn unclosed <!-- stays.\n\n```\ncode\n```\n");
    assertIncludes(out, "```html\n<!-- kept -->\n```");
    assertIncludes(out, "An unclosed <!-- stays.");
    assertIncludes(out, "```\ncode\n```");
  });

  test("comment markers in a code span are text", async () => {
    const input =
      "Use `<!--` to start and `<!-- x -->` inline.\n\n```\ncode\n```\n\nKept section.\n\n" +
      "End it with `-->` or an arrow --> here.\n";
    assert.equal(normalize(await convert(input)), normalize(input));
  });
});

/* -------------------------------------------------------------------------- */
/* Code blocks                                                                */
/* -------------------------------------------------------------------------- */

describe("code blocks", () => {
  test("@code{.c} … @endcode becomes a fenced block with the language", async () => {
    const out = await convert("@defgroup boards_g G\n@brief B.\n@code{.c}\nint x = 1;\n@endcode\n");
    assertIncludes(out, "```c\nint x = 1;\n```");
    assertExcludes(out, "@code");
    assertExcludes(out, "@endcode");
    assert.equal(parse("@defgroup boards_g G\n@brief B.\n@code{.c}\nint x = 1;\n@endcode\n").brief, "B.");
  });

  test("\\code … \\endcode without a language becomes a plain fenced block", async () => {
    const out = await convert("Text.\n\n\\code\nfoo();\n\\endcode\n\nAfter.");
    assertIncludes(out, "```\nfoo();\n```");
    assertExcludes(out, "\\code");
    assertExcludes(out, "\\endcode");
  });

  test("a code block keeps its position between the surrounding paragraphs", async () => {
    const out = await convert("Intro paragraph.\n\n@code{.c}\nint x = 1;\n@endcode\n\nTrailing paragraph.");
    assertOrder(out, "Intro paragraph.", "```c\nint x = 1;\n```", "Trailing paragraph.");
  });

  test("a page body keeps both a @note aside and a @code block", async () => {
    const out = await convert("Intro.\n\n@note Careful.\n\n@code{.c}\nint x;\n@endcode");
    assert.deepEqual(asides(out), [{ type: "note", title: undefined, body: "Careful." }]);
    assertIncludes(out, "```c\nint x;\n```");
    assertOrder(out, "Intro.", ":::note", "Careful.", ":::", "```c");
  });

  test("a \\endcode glued to the last code line still ends the block", async () => {
    const out = await convert('Steps:\n\\code\n }, {\n     "Success",\n};\\endcode tail prose\n\nAfter.');
    const { prose, code } = splitFences(out);
    assert.equal(code.length, 1, out);
    assertIncludes(code[0], '"Success",');
    assertExcludes(code[0], "tail prose");
    assertIncludes(prose, "tail prose");
    assertIncludes(prose, "After.");
    assertExcludes(out, "\\endcode");
  });

  test("a \\code{.c} glued to the word before it opens the block", async () => {
    const out = await convert(
      "Hook for the timer.\nSignature:\\code{.c}\nuint32_t my_hook(void);\n\\endcode\n- it may be necessary to use structs.",
    );
    const { prose, code } = splitFences(out);
    assert.deepEqual(code, ["uint32_t my_hook(void);"], out);
    assertIncludes(prose, "Signature:");
    assertIncludes(prose, "it may be necessary to use structs.");
    assertExcludes(out, "\\code");
    assertExcludes(out, "\\endcode");
  });

  test("a one-line @code{.c} … @endcode becomes code", async () => {
    const out = await convert("Intro.\n\n@code{.c} int x = 1; @endcode\n\nAfter.");
    const { prose, code } = splitFences(out);
    assert.deepEqual(code, ["int x = 1;"], out);
    assertExcludes(prose, "@code");
    assertExcludes(prose, "@endcode");
  });

  test("a one-line @code … @endcode keeps braced content as code, not as a language", async () => {
    const out = await convert('Intro.\n\n@code {"success": null} @endcode\n\nAfter.');
    const { prose, code } = splitFences(out);
    assert.deepEqual(code, ['{"success": null}'], out);
    assertExcludes(prose, "@code");
    assertIncludes(prose, "After.");
  });

  test("nothing inside ``` / ~~~ / @code / \\code blocks is transformed or indexed", async () => {
    const code = [
      "@defgroup boards_fake Fake",
      "@brief fake brief",
      "@ingroup boards",
      "@{",
      "<!-- not a comment here -->",
      "\\section fake_section Fake section",
      "## Heading {#fake_heading}",
      "Setext",
      "======",
      "@note not a note",
      "@see gpio_init",
      '\\ref gpio_init and @ref gpio_init "text"',
      "[text](@ref gpio_init) and [frag](#boards_other_flashing)",
      "\\anchor fake_anchor",
      "\\c foo \\p bar \\b baz \\e qux @details",
      "#GPIO_PIN and `#GPIO_PIN`",
      "<center>",
      "<br>",
      '@image html bus.svg "Bus"',
      "![x](dir/x.png)",
      "| a | b |",
      "|---|---|",
      "      | 1 | 2 |",
      "after table",
      "@}",
    ].join("\n");
    const variants: [open: string, close: string, expectedOpen: string, expectedClose: string][] = [
      ["```c", "```", "```c", "```"],
      ["~~~", "~~~", "~~~", "~~~"],
      ["@code{.c}", "@endcode", "```c", "```"],
      ["\\code", "\\endcode", "```", "```"],
    ];
    const failures: string[] = [];
    for (const [open, close, expectedOpen, expectedClose] of variants) {
      const input = `Intro.\n\n${open}\n${code}\n${close}\n\nAfter.`;
      const out = await convert(input);
      if (!out.includes(`${expectedOpen}\n${code}\n${expectedClose}`)) {
        failures.push(`${open}:\n${out}`);
      }
      const anchors = new AnchorIndex();
      collectDoxygenAnchors(parse(input), PAGE_URL, anchors);
      for (const id of ["fake_section", "fake_heading", "fake_anchor", "boards_fake"]) {
        if (anchors.lookup(id)) {
          failures.push(`${open}: anchor ${id} indexed from code`);
        }
      }
      if (parse(input).groups.length) {
        failures.push(`${open}: @defgroup in code defines a group`);
      }
    }
    assert.deepEqual(failures, []);
  });

  test("consecutive blank lines inside a fenced block survive", async () => {
    const out = await convert("Intro.\n\n```\nline one\n\n\n\nline two\n```\n\nAfter.");
    assertIncludes(out, "```\nline one\n\n\n\nline two\n```");
  });

  test("a different fence character inside a fence is content", async () => {
    const out = await convert("Intro.\n\n```\n~~~\n@note still code\n```\n\nAfter.");
    assert.deepEqual(asides(out), []);
    assertIncludes(out, "```\n~~~\n@note still code\n```");
  });

  test("a line-start @note inside a ``` fence is code", async () => {
    const out = await convert("Intro line.\n\n```\n@note not an admonition\n```");
    assert.deepEqual(asides(out), []);
    assertIncludes(out, "```\n@note not an admonition\n```");
  });

  test("a ~~~ fence protects commands, too", async () => {
    const out = await convert("Brief.\n\n~~~\n@warning nope\n~~~");
    assert.deepEqual(asides(out), []);
    assertIncludes(out, "~~~\n@warning nope\n~~~");
  });

  test("a real @note before a fence fires, the @note inside the fence does not", async () => {
    const out = await convert("Intro.\n@note see below\n\n```\ntext @note inside\n```");
    assert.deepEqual(asides(out), [{ type: "note", title: undefined, body: "see below" }]);
    assertIncludes(out, "```\ntext @note inside\n```");
  });

  test("indented code in a list item is not transformed", async () => {
    const input =
      "1. Uncomment this line:\n\n       #USEMODULE += stdio_cdc_acm\n       @note not a note\n       <center>kept</center>\n\n" +
      "2. Use #GPIO_PIN.\n\n   A paragraph of the item with #GPIO_PIN.\n";
    const out = await convert(input);
    assertIncludes(out, "       #USEMODULE += stdio_cdc_acm\n       @note not a note\n       <center>kept</center>");
    assert.deepEqual(asides(out), []);
    // the text of the list items is transformed
    assertIncludes(out, `2. Use [\`GPIO_PIN\`](${GPIO_PIN}).`);
    assertIncludes(out, `   A paragraph of the item with [\`GPIO_PIN\`](${GPIO_PIN}).`);
  });

  test("the indentation of code in a nested list item counts from that item", async () => {
    const input =
      "- outer\n  - inner\n\n    Text of the inner item, #GPIO_PIN.\n\n        code of the inner item, #GPIO_PIN\n\n" +
      "  Text of the outer item.\n\n      code of the outer item, #GPIO_PIN\n\nText.\n\n    code, #GPIO_PIN\n";
    const out = await convert(input);
    assertIncludes(out, `    Text of the inner item, [\`GPIO_PIN\`](${GPIO_PIN}).`);
    for (const code of [
      "        code of the inner item, #GPIO_PIN",
      "      code of the outer item, #GPIO_PIN",
      "    code, #GPIO_PIN",
    ]) {
      assertIncludes(out, code);
    }
  });

  test("@code without @endcode is text", async () => {
    const out = await convert("Doxygen's \\code command is not needed here.\n\n## Next {#next_id}\n\n@note A note.\n");
    assertIncludes(out, "Doxygen's \\code command is not needed here.");
    assert.deepEqual(splitFences(out).code, []);
    assert.match(out, /^## <a id="next_id"><\/a>Next$/m);
    assert.equal(asides(out).length, 1);
  });

  test("@code in a sentence does not swallow the next code block", async () => {
    const out = await convert("The \\code command starts a block.\n\n@code\nint x;\n@endcode\n\nAfter.\n");
    assertIncludes(out, "The \\code command starts a block.");
    assert.deepEqual(splitFences(out).code, ["int x;"]);
  });

  test("the language of @code may contain other characters than letters", async () => {
    const out = await convert("@code{.sh-session}\n$ make #FOO_BAR\n@endcode\n\n@code{.c++}\nint x;\n@endcode\n");
    assertIncludes(out, "```sh-session\n$ make #FOO_BAR\n```");
    assertIncludes(out, "```c++\nint x;\n```");
    assertExcludes(out, "@code");
    assertExcludes(out, "@endcode");
  });

  test("a heading ends a list, indented code below it is code", async () => {
    const out = await convert("- a\n- b\n## Build\n\n    make #GPIO_PIN\n    @note not a note\n");
    assertIncludes(out, "    make #GPIO_PIN\n    @note not a note");
    assert.deepEqual(asides(out), []);
  });

  test("a fence in a @code block does not end the block", async () => {
    const out = await convert("@code{.md}\nExample:\n```\n\\ref boards_other\n```\n@endcode\n\nAfter #GPIO_PIN.\n");
    assertIncludes(out, "````md\nExample:\n```\n\\ref boards_other\n```\n````");
    assertIncludes(out, `After [\`GPIO_PIN\`](${GPIO_PIN}).`);
  });

  test("indented code directly below a heading is code", async () => {
    const out = await convert("## Example\n    make flash #GPIO_PIN\n\nText with #GPIO_PIN.\n");
    assertIncludes(out, "## Example\n    make flash #GPIO_PIN");
    assertIncludes(out, `Text with [\`GPIO_PIN\`](${GPIO_PIN}).`);
  });
});

/* -------------------------------------------------------------------------- */
/* Sections and headings                                                      */
/* -------------------------------------------------------------------------- */

describe("sections and headings", () => {
  test("@section becomes a level 2 heading keeping its anchor", async () => {
    const out = await convert(
      "@defgroup boards_foo Foo\n@brief B.\n\nIntro.\n\n@section intro Introduction\nBody text.\n\nSee \\ref intro.",
    );
    assert.match(out, /^## (?:<a id="intro"><\/a>)Introduction$/m);
    assertIncludes(out, "Body text.");
    assertIncludes(out, "See [Introduction](#intro).");
    assertExcludes(out, "@section");
  });

  test("section commands map to heading levels below the page title", async () => {
    const out = await convert(
      "Intro.\n\n\\section s1 One\n\nA.\n\n\\subsection s2 Two\n\nB.\n\n\\subsubsection s3 Three\n\nC.\n\n\\paragraph s4 Four\n\nD.",
    );
    assert.match(out, /^## <a id="s1"><\/a>One$/m);
    assert.match(out, /^### <a id="s2"><\/a>Two$/m);
    assert.match(out, /^#### <a id="s3"><\/a>Three$/m);
    assert.match(out, /^##### <a id="s4"><\/a>Four$/m);
  });

  test("a Doxygen page: headings get ids, code stays untouched, anchors in code are not indexed", async () => {
    const unresolved: string[] = [];
    const out = await convert(
      "# Flashing guide {#flashing_guide}\n\nIntro.\n\n@section flashing_hints_openocd OpenOCD specific hints\n\n" +
        "@code{.c}\nint x = gpio_init(1); // @ref stays text here\n@endcode\n\n" +
        "```\n@anchor quoted\n```\n\nInline `riot::thread` stays code.\n\nSee \\ref quoted and \\ref flashing_hints_openocd.",
      { unresolved },
    );
    assert.match(out, /^## <a id="flashing_guide"><\/a>Flashing guide$/m);
    assert.match(out, /^## <a id="flashing_hints_openocd"><\/a>OpenOCD specific hints$/m);
    assertIncludes(out, "```c\nint x = gpio_init(1); // @ref stays text here\n```");
    assertIncludes(out, "```\n@anchor quoted\n```");
    assertIncludes(out, "Inline `riot::thread` stays code.");
    assertIncludes(out, "See `quoted` and [OpenOCD specific hints](#flashing_hints_openocd).");
    assert.deepEqual(unresolved, ["quoted"]);
  });

  test("a leading \\section is the page title: dropped, its id points to the page", async () => {
    const foo = "@defgroup boards_foo Foo Board\n@brief B.\n\n@section boards_foo_title Foo Board\n\nText.\n\n## Flashing\n\nMore.";
    const out = await convert(foo);
    assert.doesNotMatch(out, /^#+ .*Foo Board/m);
    assert.match(out, /^## Flashing$/m);
    assertOrder(out, "Text.", "## Flashing", "More.");
    const other = await convert("See \\ref boards_foo_title.", {
      pageUrl: "/boards/bar/",
      others: [{ pageUrl: PAGE_URL, content: foo }],
    });
    assertIncludes(other, "See [Foo Board](/boards/foo/).");
  });

  test("a {#label} after a heading is its id, not part of the heading text", async () => {
    const out = await convert(
      "# List of Features (Build System Entries)            {#feature-list}\n\nProse about features.\n\nSee \\ref feature-list.",
    );
    assert.match(out, /^## (?:<a id="feature-list"><\/a>)?List of Features \(Build System Entries\)$/m);
    assertExcludes(out, "{#");
    assertIncludes(out, "See [List of Features (Build System Entries)](#feature-list).");
  });

  test("a setext heading with a {#label} becomes an ATX heading with that id", async () => {
    const out = await convert(
      "Prose first.\n\nRIOT API Documentation                     {#mainpage}\n==================\n\nProse.",
    );
    assert.match(out, /^## (?:<a id="mainpage"><\/a>)?RIOT API Documentation$/m);
    assertExcludes(out, "==");
    assertExcludes(out, "{#");
  });

  test("the {#label} of a setext heading is a \\ref target", () => {
    const anchors = new AnchorIndex();
    collectDoxygenAnchors(
      parse("Prose first.\n\nBoard Selection Guide {#board-selection-guide}\n=====================\n\nProse."),
      "/boards/",
      anchors,
    );
    assert.deepEqual(anchors.lookup("board-selection-guide"), {
      url: "/boards/#board-selection-guide",
      title: "Board Selection Guide",
    });
  });

  test("a heading attribute trailer does not leak into the heading text", async () => {
    const out = await convert("# Overview {#custom-anchor}\n\nText.");
    assert.match(out, /^## (?:<a id="custom-anchor"><\/a>)?Overview$/m);
    assertExcludes(out, "{#");
  });

  test("an explicit {#id} wins, a bracketed [#a] before it stays heading text", async () => {
    const out = await convert("Intro.\n\n## Title [#a] {#b}\n\nText.");
    assert.match(out, /^## <a id="b"><\/a>Title \[#a\]$/m);
  });

  test("ATX and setext headings are shifted so the top level is h2, rules stay rules", async () => {
    await assertCases(
      [
        ["# H1\n\n## H2\n\n###### H6", "## H1\n\n### H2\n\n###### H6"],
        ["Setext H1\n=========\n\nSetext H2\n---------", "## Setext H1\n\n### Setext H2"],
        ["para\n\n---\n\nafter a thematic break", "para\n\n---\n\nafter a thematic break"],
        ["# Heading with *emph* and `code`", "## Heading with *emph* and `code`"],
      ],
      async (input) => normalize(await convert(input)),
    );
  });

  test("an explicit id that is the id of another heading gets a free id", async () => {
    // the Markdown processor gives the headings the ids `flashing` and
    // `flashing-1`, an anchor `flashing` would be the second element with
    // this id
    const input = "## Flashing\n\nFirst.\n\n## Flashing {#flashing}\n\nSee [the second](#flashing) and \\ref flashing.\n";
    const out = await convert(input);
    assert.match(out, /^## Flashing$/m);
    assert.match(out, /^## <a id="flashing-2"><\/a>Flashing$/m);
    assertExcludes(out, 'id="flashing"');
    assertIncludes(out, "See [the second](#flashing-2) and [Flashing](#flashing-2).");
    // the links to the anchor are not reported as unresolved
    const unresolved: string[] = [];
    await convert(input, { unresolved });
    assert.deepEqual(unresolved, []);
    // a reference from another page links to the heading with the explicit id
    const fromOtherPage = await convert("\\ref flashing", {
      pageUrl: "/boards/bar/",
      others: [{ pageUrl: PAGE_URL, content: input }],
    });
    assertIncludes(fromOtherPage, "[Flashing](/boards/foo/#flashing-2)");
  });

  test("a heading that has its explicit id already gets no anchor, also if its title is repeated", async () => {
    const out = await convert("## Flashing {#flashing}\n\nFirst.\n\n## Flashing\n\nSee [the first](#flashing).\n");
    assertExcludes(out, "<a id=");
    assertIncludes(out, "See [the first](#flashing).");
  });

  test("only the first heading with an explicit id gets the anchor", async () => {
    const out = await convert("## One {#same}\n\n## Two {#same}\n\nSee \\ref same.\n");
    assert.match(out, /^## <a id="same"><\/a>One$/m);
    assert.match(out, /^## Two$/m);
    assertIncludes(out, "See [One](#same).");
  });

  test("the title of a section is on the line of the command", async () => {
    const out = await convert("\\section sec_a\nSome text here.\n\n@section sec_b Title B\n");
    assertIncludes(out, "Some text here.");
    assertExcludes(out, "# Some text here.");
    assert.match(out, /^## <a id="sec_b"><\/a>Title B$/m);
  });

  test("the # that close a heading are not part of its id", async () => {
    // the Markdown processor gives the first heading the id `flashing`
    const out = await convert("## Flashing ##\n\nFirst.\n\n## Programming {#flashing}\n\nSee \\ref flashing.\n");
    assert.match(out, /^## <a id="flashing-1"><\/a>Programming$/m);
    assertIncludes(out, "See [Programming](#flashing-1).");
  });
});

describe("heading ids of the Markdown processor", () => {
  test("the headings of a page are converted for the Markdown processor", async () => {
    const input =
      "Intro.\n\n@section s1 First Steps\n\nText.\n\nSetext Title {#setext}\n============\n\n" +
      "```sh\n# not a heading\n```\n\n## Pins of \\ref boards_other ##\n";
    assert.equal(
      headingMarkdown(parse(input), await makeCtx(input)),
      "## First Steps\n\n## Setext Title\n\n### Pins of [Other Board](/boards/other/) ##",
    );
  });

  test("an explicit id is on the rendered page once", async () => {
    // the id of the processor, and another one
    for (const [heading, id] of [
      ["Foo\u00A0Bar", "foobar"],
      ["Foo\u00A0Bar", "foo-bar"],
      ["_Deep-sleep_ mode", "deep-sleep-mode"],
      ["_Deep-sleep_ mode", "_deep-sleep_-mode"],
      ["A &amp; B -- C", "a--b--c"],
      [":rocket: Launch", "-launch"],
      ["[Schematics](http://example.com/a.pdf)", "schematics"],
      ["Pins of \\ref boards_other", "pins-of-other-board"],
    ]) {
      const { code } = await processor.render(await convert(`## ${heading} {#${id}}\n`));
      assert.equal(code.split(`id="${id}"`).length - 1, 1, code);
    }
  });

  test("a heading that the processor gives its explicit id gets no anchor", async () => {
    for (const heading of ["_Deep-sleep_ mode {#deep-sleep-mode}", "A &amp; B -- C {#a--b--c}"]) {
      assertExcludes(await convert(`## ${heading}\n`), "<a id=");
    }
  });

  test("the conversion needs an id for every heading", async () => {
    const input = "## One\n\n## Two\n";
    const ctx = { ...(await makeCtx(input)), headingIds: ["one"] };
    assert.throws(() => transformDoxygenMarkdown(parse(input), ctx), /1 heading ids for 2 headings/);
  });
});

/* -------------------------------------------------------------------------- */
/* Anchors                                                                    */
/* -------------------------------------------------------------------------- */

describe("anchors", () => {
  test("an own-line \\anchor is an empty HTML anchor and a \\ref target from other pages", async () => {
    const foo = "@defgroup boards_foo Foo\n@brief Pins.\n\n@anchor gpio_modes\nThe modes a pin takes.\n";
    assertIncludes(await convert(foo), '<a id="gpio_modes"></a>\nThe modes a pin takes.');
    const other = await convert("See \\ref gpio_modes.", {
      pageUrl: "/boards/bar/",
      others: [{ pageUrl: PAGE_URL, content: foo }],
    });
    assert.deepEqual(linkTargets(other), ["/boards/foo/#gpio_modes"]);
    assertIncludes(await convert("@anchor only_a_handle"), '<a id="only_a_handle"></a>');
  });

  test("several own-line anchors mark the paragraph they precede", async () => {
    const out = await convert(
      "@anchor STDIO_UART_DEV_hint\n@anchor STDIO_UART_BAUDRATE_hint\n__STDIO_UART_DEV__ sets the device.",
    );
    assertOrder(
      out,
      '<a id="STDIO_UART_DEV_hint"></a>',
      '<a id="STDIO_UART_BAUDRATE_hint"></a>',
      "__STDIO_UART_DEV__ sets the device.",
    );
    assertExcludes(out, "@anchor");
  });

  test("an \\anchor next to @name and @{ is indexed and rendered", async () => {
    const out = await convert("@name Flags\n@anchor net_gnrc_netif_flags\n@{\n\nText.\n\n@}\n\nSee \\ref net_gnrc_netif_flags.");
    assertIncludes(out, '<a id="net_gnrc_netif_flags"></a>');
    assertIncludes(out, "See [net_gnrc_netif_flags](#net_gnrc_netif_flags).");
  });

  test("an \\anchor inside a paragraph is a position, and the command is consumed", async () => {
    const out = await convert("Before @anchor mid_hint the hint, then text.");
    assertIncludes(out, '<a id="mid_hint"></a>');
    assertExcludes(out, "@anchor");
  });

  test("the punctuation behind \\anchor is not part of the id", async () => {
    const unresolved: string[] = [];
    const out = await convert("See \\anchor foo_anchor. And \\ref foo_anchor here.\n", { unresolved });
    assertIncludes(out, 'See <a id="foo_anchor"></a>. And [foo_anchor](#foo_anchor) here.');
    assert.deepEqual(unresolved, []);
  });
});

/* -------------------------------------------------------------------------- */
/* References                                                                 */
/* -------------------------------------------------------------------------- */

describe("references", () => {
  test("@ref and \\ref become links, no command survives as text", async () => {
    const out = await convert("This guide covers @ref gpio_init and \\ref gpio_init in prose, plus @b bold words.");
    assert.equal(
      out.trim(),
      `This guide covers [\`gpio_init\`](${GPIO_INIT}) and [\`gpio_init\`](${GPIO_INIT}) in prose, plus **bold** words.`,
    );
  });

  test("a multi-line @note with a @ref is one aside with a link", async () => {
    const out = await convert("@note Pins are configured before the start\nof the thread; see @ref gpio_init.\n\nAfter.");
    assert.deepEqual(asides(out), [
      { type: "note", title: undefined, body: `Pins are configured before the start\nof the thread; see [\`gpio_init\`](${GPIO_INIT}).` },
    ]);
  });

  test("@ref with parens links the function, @p is code, @a is emphasis", async () => {
    const out = await convert("See @ref gpio_init() and pass @p pin; emphasis @a here.");
    assert.match(
      out,
      new RegExp(`^See \\[\`gpio_init(?:\\(\\))?\`\\]\\(${GPIO_INIT.replace(/[.?]/g, "\\$&")}\\) and pass \`pin\`; emphasis \\*here\\*\\.$`, "m"),
    );
  });

  test("a hyphenated group id is one \\ref target", async () => {
    const out = await convert("See @ref boards_arduino-due and @ref boards_samr21-xpro.");
    assert.equal(
      out.trim(),
      "See [Arduino Due](/boards/arduino-due/) and [Atmel SAM R21 Xplained Pro](/boards/samr21-xpro/).",
    );
  });

  test("without a tag file, a hyphenated group id links to its API group page", async () => {
    const out = await convert("See @ref boards_unknown-board and @ref gpio_init.", { isolated: true, api: false });
    assert.deepEqual(linkTargets(out), [`${API}/group__boards__unknown-board.html`]);
    assertIncludes(out, "`gpio_init`");
  });

  test("[text](@ref id) links to the target, an unresolved one degrades to its text", async () => {
    assertIncludes(
      await convert("See the [drivers](@ref net_unicoap_drivers) module."),
      `See the [drivers](${UNICOAP_DRIVERS}) module.`,
    );
    assertIncludes(await convert("See the [Due](@ref boards_arduino-due)."), "See the [Due](/boards/arduino-due/).");
    assert.equal((await convert("x [label](@ref nope) y")).trim(), "x label y");
  });

  test("a @ref directly after the punctuation of another @ref lowers too", async () => {
    const out = await convert("@ref boards_arduino-due.@ref boards_samr21-xpro");
    assert.equal(out.trim(), "[Arduino Due](/boards/arduino-due/).[Atmel SAM R21 Xplained Pro](/boards/samr21-xpro/)");
  });

  test("a @ref inside prose parentheses links", async () => {
    const out = await convert("Use @ref gpio_init (see @ref GPIO_PIN).");
    assert.equal(out.trim(), `Use [\`gpio_init\`](${GPIO_INIT}) (see [\`GPIO_PIN\`](${GPIO_PIN})).`);
  });

  test("a @ref inside prose brackets (an interval) links", async () => {
    const out = await convert("Clamps to [@ref PHYDAT_MIN, @ref PHYDAT_MAX] and more.");
    assert.deepEqual(linkTargets(out), [PHYDAT_MIN, PHYDAT_MAX]);
    assertExcludes(out, "@ref");
  });

  test("a @ref inside the text of a Markdown link does not nest a link", async () => {
    const out = await convert("Read [see @ref gpio_init](https://e.com) first.");
    assert.deepEqual(linkTargets(out), ["https://e.com"]);
  });

  test("an inline command opens after any non-word character", async () => {
    const out = await convert("Increments *\\p ctry; ex: (HSE_VALUE,@ref LL_Divider() done.");
    assertIncludes(out, "*`ctry`;");
    assert.deepEqual(linkTargets(out), [LL_DIVIDER]);
    assertExcludes(out, "\\p ");
    assertExcludes(out, "@ref");
  });

  test("a command glued to a word stays text", async () => {
    assert.equal((await convert("Ask user@ref stays here.")).trim(), "Ask user@ref stays here.");
  });

  test("e-mail addresses and Windows paths are not commands", async () => {
    await assertCases(
      [
        ["Contact john@example.org for details.", "Contact john@example.org for details."],
        ["Contact foo@note.example is fine.", "Contact foo@note.example is fine."],
        ["See C:\\notes for details.", "See C:\\notes for details."],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });

  test("an unresolved @ref degrades to code, with text to the text, and is reported", async () => {
    const unresolved: string[] = [];
    assert.equal((await convert("x @ref nope y", { unresolved })).trim(), "x `nope` y");
    assert.equal((await convert('x @ref nope2 "Nope text" y', { unresolved })).trim(), "x Nope text y");
    assert.deepEqual(unresolved, ["nope", "nope2"]);
  });

  test("references resolve inside emphasis, list items and block quotes", async () => {
    const out = await convert("a *@ref gpio_init* b\n\n- see \\ref gpio_init\n\n> ref @ref gpio_init");
    assertIncludes(out, `a *[\`gpio_init\`](${GPIO_INIT})* b`);
    assertIncludes(out, `- see [\`gpio_init\`](${GPIO_INIT})`);
    assertIncludes(out, `> ref [\`gpio_init\`](${GPIO_INIT})`);
  });

  test("ordinary links and links in fenced code are left alone", async () => {
    assert.equal((await convert("[home](https://x)")).trim(), "[home](https://x)");
    assert.equal((await convert("[t](#no_such_anchor)")).trim(), "[t](#no_such_anchor)");
    assertIncludes(await convert("```\n[t](@ref gpio_init)\n```"), "```\n[t](@ref gpio_init)\n```");
  });

  test("[text](#id) to an anchor of another page becomes an absolute link, a local one stays", async () => {
    const out = await convert("a [g](#boards_other_flashing) b and [h](#local_sec).\n\n## Local {#local_sec}\n\nText.");
    assertIncludes(out, "a [g](/boards/other/#boards_other_flashing) b and [h](#local_sec).");
  });

  test("a clickable image keeps its content and rewrites the @ref target", async () => {
    const out = await convert("[![alt](logo.png)](@ref boards_other)");
    assert.equal(out.trim(), "[![alt](/img/logo.png)](/boards/other/)");
  });

  test("a reference prefers the anchor of its own page", async () => {
    const own = { pageUrl: PAGE_URL, content: "## Flashing Foo {#flashing}\n\nSee [flashing](#flashing) and \\ref flashing.\n" };
    const other = { pageUrl: "/boards/zzz/", content: "@defgroup boards_zzz ZZZ\n\n## Flashing ZZZ {#flashing}\n\nText.\n" };
    // the result doesn't depend on the order in which the pages are indexed
    for (const pages of [
      [own, other],
      [other, own],
    ]) {
      const anchors = new AnchorIndex();
      for (const page of pages) {
        collectDoxygenAnchors(parse(page.content), page.pageUrl, anchors);
      }
      const out = transformDoxygenMarkdown(parse(own.content), { ...(await makeCtx(own.content)), anchors });
      assertIncludes(out, "See [flashing](#flashing) and [Flashing Foo](#flashing).");
      // the id is reported, a third page links to the first definition
      assert.deepEqual(anchors.duplicates(), [["flashing", pages.map((page) => `${page.pageUrl}#flashing`)]]);
      assert.equal(anchors.lookup("flashing", "/boards/bar/")?.url, `${pages[0].pageUrl}#flashing`);
    }
  });

  test("a link to a heading of the page is not redirected to an anchor of another page", async () => {
    const other = {
      pageUrl: "/boards/zzz/",
      content: "@defgroup boards_zzz ZZZ\n\n## Flashing ZZZ {#flashing}\n\n## Only {#only_zzz}\n\nText.\n",
    };
    // `## Flashing` has no explicit id, the Markdown processor gives it `flashing`
    const out = await convert("## Flashing\n\nSee [flashing](#flashing) and [other](#only_zzz).\n", { others: [other] });
    assertIncludes(out, "See [flashing](#flashing) and [other](/boards/zzz/#only_zzz).");
  });

  test("a reference in the label of a link is text, the link is kept", async () => {
    const unresolved: string[] = [];
    const out = await convert(
      '[see @ref gpio_init](https://e.com), [see \\ref drivers_periph_gpio "the GPIO API"](https://e.com) and [see @ref nope](https://e.com)\n',
      { unresolved },
    );
    assertIncludes(
      out,
      "[see `gpio_init`](https://e.com), [see the GPIO API](https://e.com) and [see `nope`](https://e.com)",
    );
    assert.deepEqual(linkTargets(out), ["https://e.com", "https://e.com", "https://e.com"]);
    assert.deepEqual(unresolved, ["nope"]);
  });

  test("a link to an anchor that does not exist is reported", async () => {
    const unresolved: string[] = [];
    const out = await convert("## Overview\n\nSee [above](#overview) and [nothing](#no_such_anchor).\n", { unresolved });
    // a link to a heading of the page is not an error
    assertIncludes(out, "See [above](#overview) and [nothing](#no_such_anchor).");
    assert.deepEqual(unresolved, ["#no_such_anchor"]);
  });

  test("a reference to a group without a title shows its name", async () => {
    const other = { pageUrl: "/boards/untitled/", content: "@defgroup boards_untitled\n@ingroup boards\n\nText.\n" };
    assertIncludes(await convert("See \\ref boards_untitled.", { others: [other] }), "See [boards_untitled](/boards/untitled/).");
  });
});

/* -------------------------------------------------------------------------- */
/* Code spans                                                                 */
/* -------------------------------------------------------------------------- */

describe("code spans", () => {
  test("a command inside a code span is literal", async () => {
    assert.equal((await convert("Literal `@ref x`, and @b bold text.")).trim(), "Literal `@ref x`, and **bold** text.");
    assert.equal((await convert("Literal `\\c foo` and `[t](@ref gpio_init)`.")).trim(), "Literal `\\c foo` and `[t](@ref gpio_init)`.");
  });

  test("a callout command inside a code span stays literal", async () => {
    await assertCases(
      [
        ["Text with `@note x` literal.", "Text with `@note x` literal."],
        ["Text `` @note x `` done.", "Text `` @note x `` done."],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });

  test("autolinking skips code spans that are not a known #symbol or call", async () => {
    const input =
      "Call `#include <stdio.h>`, `#FOO_UNKNOWN`, `GPIO_PIN`, `foo_unknown()` and `gpio_init(pin, mode)` directly.";
    assert.equal((await convert(input)).trim(), input);
  });
});

/* -------------------------------------------------------------------------- */
/* Callouts                                                                   */
/* -------------------------------------------------------------------------- */

describe("callouts", () => {
  test("every callout command, in @ and \\ form, maps to its aside type", async () => {
    const kinds: [command: string, aside: string][] = [
      ["note", ":::note"],
      ["remark", ":::note"],
      ["remarks", ":::note"],
      ["see", ":::note[See also]"],
      ["sa", ":::note[See also]"],
      ["tip", ":::tip"],
      ["hint", ":::tip"],
      ["warning", ":::caution"],
      ["attention", ":::caution"],
      ["important", ":::danger"],
      ["experimental", ":::caution[Experimental]"],
    ];
    const cases: [string, string][] = [];
    for (const [command, aside] of kinds) {
      for (const prefix of ["@", "\\"]) {
        cases.push([`Intro.\n\n${prefix}${command} Body text.\n\nAfter.`, `Intro.\n\n${aside}\nBody text.\n:::\n\nAfter.`]);
      }
    }
    await assertCases(cases, async (input) => normalize(await convert(input)));
  });

  test("a @note paragraph ends at the blank line", async () => {
    const out = await convert("@note A note\nthat continues.\n\nBody text.");
    assert.deepEqual(asides(out), [{ type: "note", title: undefined, body: "A note\nthat continues." }]);
    assertOrder(out, ":::note", "that continues.", ":::", "Body text.");
  });

  test("a @warning between paragraphs stays in place", async () => {
    const out = await convert("First paragraph.\n\n@warning Be careful.\n\nSecond paragraph.");
    assert.equal(normalize(out), "First paragraph.\n\n:::caution\nBe careful.\n:::\n\nSecond paragraph.");
  });

  test("a @note takes its whole paragraph", async () => {
    const out = await convert("@note arg0 arg1\narg7 arg8\narg9");
    assert.deepEqual(asides(out), [{ type: "note", title: undefined, body: "arg0 arg1\narg7 arg8\narg9" }]);
  });

  test("prose before a @note stays outside the aside", async () => {
    const out = await convert("Hello, World!\n\n@note arg0");
    assert.equal(normalize(out), "Hello, World!\n\n:::note\narg0\n:::");
  });

  test("a leading @note gives exactly one aside", async () => {
    assert.equal(normalize(await convert("@note just a note")), ":::note\njust a note\n:::");
  });

  test("a @warning after a @note, and a @note after prose, are separate asides", async () => {
    const out = await convert("@note First.\n@warning Careful.\n\nProse.\n\n@note Third.");
    assert.deepEqual(
      asides(out).map((a) => [a.type, a.body]),
      [
        ["note", "First."],
        ["caution", "Careful."],
        ["note", "Third."],
      ],
    );
    assertOrder(out, "Careful.", "Prose.", "Third.");
  });

  test("a @note directly after a @note joins it as a second paragraph", async () => {
    const out = await convert("@note First.\n@note Second.\n\nProse.");
    assert.deepEqual(
      asides(out).map((a) => [a.type, a.body]),
      [["note", "First.\n\nSecond."]],
    );
  });

  test("a fence directly below a @note line belongs to the note", async () => {
    const out = await convert("@note Clear it:\n~~~~~~~~{.c}\nreset(&dev);\n~~~~~~~~\n\nAfter.");
    const notes = asides(out);
    assert.equal(notes.length, 1, out);
    assertIncludes(notes[0].body, "Clear it:");
    assertIncludes(notes[0].body, "reset(&dev);");
    assertOrder(out, ":::note", "reset(&dev);", ":::", "After.");
  });

  test("a callout with only a code block below the command keeps the code block", async () => {
    const out = await convert("@note Example:\n      @code\n      make flash\n      make term\n      @endcode\n\nAfter.\n");
    assertIncludes(out, ":::note\nExample:\n```\nmake flash\nmake term\n```\n:::");
  });

  test("a callout in a list item ends at the next item", async () => {
    const out = await convert("1. Configure:\n   @note careful\n2. Next\n3. Last\n");
    assertIncludes(out, "   :::note\n   careful\n   :::");
    assertOrder(out, ":::note", ":::", "2. Next\n3. Last");
  });
});

/* -------------------------------------------------------------------------- */
/* See also                                                                   */
/* -------------------------------------------------------------------------- */

describe("see also", () => {
  /** Link targets of all "See also" asides of `md`. */
  const seeTargets = (md: string) =>
    asides(md)
      .filter((a) => a.title === "See also")
      .flatMap((a) => linkTargets(a.body));

  test("a bare @see / @sa reference list autolinks each name", async () => {
    const out = await convert("@see gpio_read(), gpio_write\n@sa gpio_t");
    assert.deepEqual(seeTargets(out), [GPIO_READ, GPIO_WRITE, GPIO_T]);
    assertExcludes(out, "@ref");
  });

  test("the period after a @see name ends the sentence, not the name", async () => {
    const out = await convert("@see gpio_read.");
    assert.deepEqual(asides(out), [{ type: "note", title: "See also", body: `[\`gpio_read\`](${GPIO_READ}).` }]);
  });

  test("a trailing comma does not demote the reference list", async () => {
    assert.deepEqual(seeTargets(await convert("@see gpio_read, gpio_write,")), [GPIO_READ, GPIO_WRITE]);
  });

  test("@sa #MACRO(a, b, c) is one reference, not one per argument", async () => {
    assert.deepEqual(seeTargets(await convert("@sa #EXPECT_EQ(expected, actual, msg)")), [EXPECT_EQ]);
  });

  test("a @sa joined by prose links each call and keeps the words", async () => {
    const out = await convert("@sa gpio_read() and gpio_write()");
    assert.deepEqual(seeTargets(out), [GPIO_READ, GPIO_WRITE]);
    assertIncludes(out, ") and [");
  });

  test("a bare URL after @see is kept verbatim", async () => {
    assert.deepEqual(asides(await convert("@see https://example.com/a/b")), [
      { type: "note", title: "See also", body: "https://example.com/a/b" },
    ]);
  });

  test("@see is an aside in place; @see @ref X links X without a stray @ref", async () => {
    const out = await convert("Before.\n\n@see gpio_read, gpio_write\n@see @ref boards_other\n\nAfter.");
    assert.deepEqual(seeTargets(out), [GPIO_READ, GPIO_WRITE, "/boards/other/"]);
    assertExcludes(out, "@ref");
    assertOrder(out, "Before.", ":::note[See also]", "After.");
  });

  test("see also: a Markdown link stays as it is", async () => {
    const out = await convert("@see [the gpio_init docs](https://e.com), gpio_init\n");
    assert.equal(asides(out)[0].body, `[the gpio_init docs](https://e.com), [\`gpio_init\`](${GPIO_INIT})`);
  });
});

/* -------------------------------------------------------------------------- */
/* HTML blocks and tables                                                     */
/* -------------------------------------------------------------------------- */

describe("HTML blocks", () => {
  test("<center> tags go, the table inside stays a table", async () => {
    const out = await convert(
      'Parameters:\n\n<center>\n| A | B |\n|:--|:--|\n| 1 | \\ref boards_other "t" |\n</center>\n\nAfter.',
    );
    assertExcludes(out, "center");
    assertIncludes(out, "| A | B |\n|:--|:--|\n| 1 | [t](/boards/other/) |");
    assertOrder(out, "Parameters:", "| A | B |", "After.");
  });

  test("inline and block HTML pass through, a <br>-only line becomes a blank line", async () => {
    await assertCases(
      [
        ["a paragraph with <em>inline html</em> in it", "a paragraph with <em>inline html</em> in it"],
        ['<div class="note">\nblock html\n</div>', '<div class="note">\nblock html\n</div>'],
        ["text with an <br> inline tag", "text with an <br> inline tag"],
        ["Line one.\n<br>\nLine two.", "Line one.\n\nLine two."],
      ],
      async (input) => normalize(await convert(input)),
    );
  });
});

describe("tables", () => {
  test("a GFM table ends at the first line without |", async () => {
    const out = await convert("| Name | Value |\n|:-----|------:|\n| a    | 1     |\n| b    | 2     |\nAfter the table.");
    assertIncludes(out, "| Name | Value |\n|:-----|------:|\n| a    | 1     |\n| b    | 2     |\n\nAfter the table.");
  });

  test("a table with a short |:--| delimiter row also ends at the first line without |", async () => {
    const out = await convert("| A | B |\n|:--|:--|\n| 1 | 2 |\nAfter.");
    assertIncludes(out, "| 1 | 2 |\n\nAfter.");
  });

  test("table cells keep inline markup and convert inline commands", async () => {
    assert.equal((await convert("| Col |\n|-----|\n| `code` and *emph* |")).trim(), "| Col |\n|-----|\n| `code` and *emph* |");
    assertIncludes(await convert("| A | B |\n|---|---|\n| \\c foo | \\b bar |"), "| `foo` | **bar** |");
  });

  test("a single column table ends at the first line without |", async () => {
    const out = await convert("| Col |\n|-----|\n| `code` |\nAfter.");
    assertIncludes(out, "| `code` |\n\nAfter.");
  });

  test("plain GFM tables and strikethrough pass through unchanged", async () => {
    await assertCases(
      [
        ["| a | b |\n| --- | --- |\n| 1 | 2 |", "| a | b |\n| --- | --- |\n| 1 | 2 |"],
        ["| left | center | right |\n| :--- | :---: | ---: |\n| l | c | r |", "| left | center | right |\n| :--- | :---: | ---: |\n| l | c | r |"],
        ["| x | y |\n| --- | --- |\n| a `code` | b **bold** |", "| x | y |\n| --- | --- |\n| a `code` | b **bold** |"],
        ["| pipe | in cell |\n| --- | --- |\n| a \\| b | c |", "| pipe | in cell |\n| --- | --- |\n| a \\| b | c |"],
        ["| A | B |\n|---|---|\n| 1 | 2 |\n\n~~gone~~ and normal.", "| A | B |\n|---|---|\n| 1 | 2 |\n\n~~gone~~ and normal."],
      ],
      async (input) => normalize(await convert(input)),
    );
  });

  test("a cell delimiter ends the word of an inline command", async () => {
    const out = await convert("| A | B |\n|---|---|\n| \\c FOO| \\b bar |\n| \\c A\\|B| x |\n\nNot a table: \\c A|B.\n");
    assertIncludes(out, "| `FOO` | **bar** |");
    assertIncludes(out, "| `A\\|B` | x |");
    assertIncludes(out, "Not a table: `A|B`.");
  });
});

/* -------------------------------------------------------------------------- */
/* Inline commands                                                            */
/* -------------------------------------------------------------------------- */

describe("inline commands", () => {
  test("@p word becomes a code span", async () => {
    assert.equal((await convert("This converts @p arg0 by inverting it.")).trim(), "This converts `arg0` by inverting it.");
  });

  test("\\c \\p \\b \\e \\em \\a, in @ and \\ form, become code, bold and emphasis", async () => {
    const cases: [string, string][] = [];
    for (const prefix of ["@", "\\"]) {
      cases.push(
        [`x ${prefix}c word y`, "x `word` y"],
        [`x ${prefix}p word y`, "x `word` y"],
        [`x ${prefix}b word y`, "x **word** y"],
        [`x ${prefix}e word y`, "x *word* y"],
        [`x ${prefix}em word y`, "x *word* y"],
        [`x ${prefix}a word y`, "x *word* y"],
      );
    }
    await assertCases(cases, async (input) => (await convert(input)).trim());
  });

  test("@details text on the command line opens the description, aligned lines stay prose", async () => {
    const input =
      "@defgroup boards_g G\n@brief           Destroys a pthread_mutexattr_t\n" +
      "@details         There is no need to ever call this function.\n" +
      "                 This function is a no-op.\n";
    assert.equal(parse(input).brief, "Destroys a pthread_mutexattr_t");
    const out = await convert(input);
    assertExcludes(out, "@details");
    assert.match(out, /^There is no need to ever call this function\.\s+This function is a no-op\.$/m);
    assert.deepEqual(splitFences(out).code, []);
  });

  test("a typewriter word may start with *, & or (", async () => {
    await assertCases(
      [
        ["Returns 0 if \\p *result is set.", "Returns 0 if `*result` is set."],
        ["the first @p (*output_length) bytes", "the first `(*output_length)` bytes"],
        ["and \\c &value, then", "and `&value`, then"],
        ["then @b *bold. done", "then **\\*bold**. done"],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });

  test("a typewriter word runs to the next space, sentence punctuation stays prose", async () => {
    await assertCases(
      [
        ["Named @c <PAD>_<SIGNAL>. Prefer", "Named `<PAD>_<SIGNAL>`. Prefer"],
        ["Prefer @p #if over", "Prefer `#if` over"],
        ["over @p || here", "over `||` here"],
        ["an absolute @c / path", "an absolute `/` path"],
        ["(not portable @c pmu_*() primitives)", "(not portable `pmu_*()` primitives)"],
        ["and \\b * marks.", "and **\\*** marks."],
        ["Set \\c FOO.", "Set `FOO`."],
        ["(see \\c FOO)", "(see `FOO`)"],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });
});

/* -------------------------------------------------------------------------- */
/* Symbol autolinks                                                           */
/* -------------------------------------------------------------------------- */

describe("#symbol autolinks", () => {
  test("#SYMBOL known to the tag file links, a bare #include / #42 / #define does not", async () => {
    const out = await convert("Set #GPIO_PIN and #gpio_init() here. Do not touch #include or #42 or #define.");
    assert.equal(
      out.trim(),
      `Set [\`GPIO_PIN\`](${GPIO_PIN}) and [\`gpio_init()\`](${GPIO_INIT}) here. Do not touch #include or #42 or #define.`,
    );
    assert.equal((await convert("Unknown #FOO_BAR macro.")).trim(), "Unknown `FOO_BAR` macro.");
    assert.equal((await convert("A [pins](#GPIO_PIN) link.")).trim(), "A [pins](#GPIO_PIN) link.");
  });

  test("a #SYMBOL inside the text of a Markdown link does not nest a link", async () => {
    const out = await convert("See [the #GPIO_PIN helper](http://x) docs.");
    assert.deepEqual(linkTargets(out), ["http://x"]);
  });

  test("a bare URL is one token, also with a #fragment", async () => {
    const input =
      "Copied from https://www.arduino.cc/en/Reference/Wire. See (www.wemos.cc/d1) or <http://x.org/a.h> " +
      "and https://example.com/page#GPIO_PIN.";
    assert.equal((await convert(input)).trim(), input);
  });

  test("a symbol in the label of a link is not linked again", async () => {
    const out = await convert(
      "[see #GPIO_PIN here](https://e.com), [`#GPIO_PIN`](https://e.com) and [![pins of #GPIO_PIN](pins.png)](https://e.com)\n",
    );
    assertIncludes(out, "[see `GPIO_PIN` here](https://e.com), [`#GPIO_PIN`](https://e.com) and [![pins of `GPIO_PIN`](");
    assertExcludes(out, GPIO_PIN);
  });

  test("the arguments of a #MACRO are part of its link, those of a function are text", async () => {
    await assertCases(
      [
        ["Use #GPIO_PIN(0, 1) here.", `Use [\`GPIO_PIN(0, 1)\`](${GPIO_PIN}) here.`],
        ["Use #gpio_init(pin, mode) here.", `Use [\`gpio_init\`](${GPIO_INIT})(pin, mode) here.`],
        ["Unknown #FOO_BAR(0) macro.", "Unknown `FOO_BAR(0)` macro."],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });
});

describe("call autolinks", () => {
  test("a call of a known function or macro links without #", async () => {
    await assertCases(
      [
        ["Call gpio_init() first.", `Call [\`gpio_init()\`](${GPIO_INIT}) first.`],
        ["The pin GPIO_PIN(0, 1) is used.", `The pin [\`GPIO_PIN(0, 1)\`](${GPIO_PIN}) is used.`],
        ["(see gpio_read(), gpio_write())", `(see [\`gpio_read()\`](${GPIO_READ}), [\`gpio_write()\`](${GPIO_WRITE}))`],
        ["**gpio_init()** is required.", `**[\`gpio_init()\`](${GPIO_INIT})** is required.`],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });

  test("a call in a table cell links up to its closing parenthesis", async () => {
    const out = await convert("Pin | Function\n:---|:---\nPA5 |GPIO_PIN(0, 5):CLK\n");
    assertIncludes(out, `PA5 |[\`GPIO_PIN(0, 5)\`](${GPIO_PIN}):CLK`);
  });

  test("a name without parentheses is not a call", async () => {
    const input = "Call gpio_init first, then set GPIO_PIN and LL_Divider.";
    assert.equal((await convert(input)).trim(), input);
  });

  test("the arguments of a function are not a reference, an unknown call is text", async () => {
    const unresolved: string[] = [];
    const input = "Call gpio_init(pin, mode) from main() and foo_unknown(0).";
    assert.equal((await convert(input, { unresolved })).trim(), input);
    assert.deepEqual(unresolved, []);
  });

  test("a name in a longer word, in a path or in a URL is not a call", async () => {
    const input =
      "Not my-gpio_init(), dev.gpio_init(), drivers/gpio_init(), x=gpio_init(), \"gpio_init()\" " +
      "or https://example.com/gpio_init() and https://example.com/?q=gpio_init().";
    assert.equal((await convert(input)).trim(), input);
  });

  test("a code span with a call becomes a link", async () => {
    const out = await convert("Call `gpio_init()` for `GPIO_PIN(0, 1)`.\n");
    assertIncludes(out, `Call [\`gpio_init()\`](${GPIO_INIT}) for [\`GPIO_PIN(0, 1)\`](${GPIO_PIN}).`);
  });

  test("a call that is a link or code already is not linked again", async () => {
    await assertCases(
      [
        ["See \\ref gpio_init() here.", `See [\`gpio_init()\`](${GPIO_INIT}) here.`],
        ["See #gpio_init() here.", `See [\`gpio_init()\`](${GPIO_INIT}) here.`],
        ["See \\c gpio_init() here.", "See `gpio_init()` here."],
        ["See \\c (gpio_init()) here.", "See `(gpio_init())` here."],
        ["See [`gpio_init()`](https://e.com) here.", "See [`gpio_init()`](https://e.com) here."],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });

  test("a call in the label of a link does not nest a link", async () => {
    const out = await convert("See [the gpio_init() helper](http://x) docs.");
    assert.deepEqual(linkTargets(out), ["http://x"]);
    assertIncludes(out, "[the `gpio_init()` helper](http://x)");
  });

  test("a call in an HTML tag is not linked", async () => {
    const input = '<img src="/img/timing.png" alt="Timing of gpio_init() and GPIO_PIN(0, 1)">';
    assert.equal((await convert(input)).trim(), input);
  });

  test("a @see list links a macro call with its arguments", async () => {
    const out = await convert("@see gpio_read(), GPIO_PIN(0, 1)");
    assert.deepEqual(asides(out), [
      {
        type: "note",
        title: "See also",
        body: `[\`gpio_read()\`](${GPIO_READ}), [\`GPIO_PIN(0, 1)\`](${GPIO_PIN})`,
      },
    ]);
  });

  test("without a tag file, a call stays text", async () => {
    const input = "Call gpio_init() and `gpio_init()` for GPIO_PIN(0, 1).";
    assert.equal((await convert(input, { api: false })).trim(), input);
  });
});

/* -------------------------------------------------------------------------- */
/* Images                                                                     */
/* -------------------------------------------------------------------------- */

describe("images", () => {
  test("@image html becomes an image at its position", async () => {
    const out = await convert("Lead-in:\n@image html bus.svg\n\nAfter.");
    assert.match(out, /!\[[^\]]+\]\(\/img\/bus\.svg\)/);
    assertOrder(out, "Lead-in:", "](/img/bus.svg)", "After.");
    assertExcludes(out, "@image");
  });

  test("@image latex is dropped", async () => {
    // captions on the command lines, so that only the format is under test
    const out = await convert('Lead-in:\n\n@image html bus.svg "Bus"\n@image latex bus.eps "Bus"\n\nAfter.');
    assertIncludes(out, "![Bus](/img/bus.svg)");
    assertExcludes(out, "bus.eps");
    assertExcludes(out, "@image");
    assertOrder(out, "Lead-in:", "![Bus](/img/bus.svg)", "After.");
  });

  test("@image caption with an inline command becomes the alt text, size options go", async () => {
    const out = await convert('@image html bus.svg "A @c bus" width=50%');
    assert.match(out.trim(), /^!\[A `?bus`?\]\(\/img\/bus\.svg\)$/);
  });

  test("an uncaptioned image in a subdirectory gets an alt text and the image path", async () => {
    assert.match((await convert("@image html sub/dir/bus.svg")).trim(), /^!\[[^\]]+\]\(\/img\/bus\.svg\)$/);
  });

  test("Markdown and HTML image paths are rewritten, absolute URLs are kept", async () => {
    await assertCases(
      [
        ["an ![image](img.png) inline", "an ![image](/img/img.png) inline"],
        ["an ![image](doc/pics/board.jpg) inline", "an ![image](/img/board.jpg) inline"],
        ["an ![image](https://e.com/a.png) inline", "an ![image](https://e.com/a.png) inline"],
        ['<img src="doc/pic.jpg" alt="x">', '<img src="/img/pic.jpg" alt="x">'],
      ],
      async (input) => (await convert(input)).trim(),
    );
  });
});

/* -------------------------------------------------------------------------- */
/* Plain Markdown passes through                                              */
/* -------------------------------------------------------------------------- */

describe("plain Markdown passes through", () => {
  const identity = (cases: string[]) =>
    assertCases(
      cases.map((md) => [md, normalize(md)]),
      async (input) => normalize(await convert(input)),
    );

  test("inline constructs", async () => {
    await identity([
      "plain paragraph of text",
      "*emphasis* and **strong** and ~~struck~~ and `code`",
      "nested **strong *and emph* together**",
      'a [link](https://example.com) and [titled](https://x "the title")',
      "text with a soft\nbreak folds to a space",
      "hard break here\\\nsecond line",
      "inline code with a backtick: `` a`b ``",
    ]);
  });

  test("literal text that looks like markup", async () => {
    await identity([
      "a literal \\* star and \\_ underscore",
      "literal backtick \\` and bracket \\[ here",
      "an ampersand & and a less-than < in prose",
      "escaped hash \\# not a heading",
      "text with 2 \\* 3 = 6 math-ish stars",
      "costs $5 and $10 total",
      "use `$PATH` in code",
      "```make\nUSEMODULE += $(filter periph_%,$(FEATURES_USED))\n```",
    ]);
  });

  test("code blocks", async () => {
    await identity([
      "```\nplain fenced\n```",
      "```sh\nmake BOARD=native flash\n```",
      "```\ncode with ``` backticks inside\n```",
      "    indented code block\n    second line",
      "```c\nint x = 1; // a & b < c\n```",
      '```ts title="src/index.ts"\nconst a = 1;\n```',
    ]);
  });

  test("block quotes", async () => {
    await identity(["> a quote\n> spanning lines", "> outer\n>\n> > nested quote", "> quote with **strong** and a [link](x)"]);
  });

  test("lists", async () => {
    await identity([
      "- a\n- b\n- c",
      "- loose\n\n- list\n\n- items",
      "1. one\n2. two\n3. three",
      "3. starts at three\n4. four",
      "- outer\n  - nested\n  - items\n- back to outer",
      "- item with a paragraph\n\n  and a second paragraph in the item",
      "1. ordered with `code` and *emph*",
    ]);
  });

  test("realistic prose with a heading, code, list, quote and table", async () => {
    await identity([
      "Drive a GPIO pin.\n\n" +
        "Use [`gpio_init`](https://example.com/gpio) to configure a pin, then `gpio_set`.\n\n" +
        "## Example\n\n" +
        "```c\ngpio_init(GPIO_PIN(0, 1), GPIO_OUT);\n```\n\n" +
        "- `GPIO_IN` — input\n- `GPIO_OUT` — output\n\n" +
        "> **Note:** not all pins support interrupts.\n\n" +
        "| Mode | Value |\n| --- | --- |\n| in | 0 |\n| out | 1 |",
    ]);
  });
});

describe("edge cases", () => {
  test("see also: a name after \\ref or @ref is not referenced twice", async () => {
    const out = await convert('@see \\ref boards_other\n\n@sa @ref boards_other "the other"\n');
    assert.match(out, /^\[Other Board\]\(\/boards\/other\/\)$/m);
    assert.match(out, /^\[the other\]\(\/boards\/other\/\)$/m);
    assert.doesNotMatch(out, /[@\\]ref/);
  });

  test("see also: a bare name is referenced", async () => {
    const out = await convert("@see boards_other, more text\n");
    assertIncludes(out, "[Other Board](/boards/other/), more text");
  });

  test("heading ids: slugs are counted across code blocks", async () => {
    const out = await convert("## Flashing\n\n```\nmake flash\n```\n\n## Flashing {#flashing}\n");
    // the second heading gets the slug `flashing-1`, so the anchor is needed.
    // The first heading has the id `flashing`, so the anchor gets a free id.
    assert.match(out, /^## <a id="flashing-2"><\/a>Flashing$/m);
  });

  test("heading ids: no anchor for a unique id that equals the slug", async () => {
    const out = await convert("## Flashing the board {#flashing-the-board}\n");
    assert.match(out, /^## Flashing the board$/m);
  });

  test("callouts end at a heading", async () => {
    const out = await convert("@note this is a note\n## Next Section\ntext\n");
    assertIncludes(out, ":::note\nthis is a note\n:::\n\n## Next Section\ntext");
  });

  test("callouts end at a table", async () => {
    const out = await convert("@note this is a note\n| a | b |\n|---|---|\n| 1 | 2 |\n");
    assertIncludes(out, ":::note\nthis is a note\n:::\n\n| a | b |");
  });

  test("callouts keep a list that follows the command", async () => {
    const out = await convert("@note\n- one\n- two\n");
    assertIncludes(out, ":::note\n- one\n- two\n:::");
  });

  test("callout text aligned with the command is dedented next to a code block", async () => {
    const out = await convert("@note   Flash it\n        with this command:\n@code\nmake flash\n@endcode\n");
    assertIncludes(out, ":::note\nFlash it\nwith this command:\n```\nmake flash\n```\n:::");
  });

  test("blank lines in code blocks are kept", async () => {
    const out = await convert("```c\nint a;\n\n\nint b;\n```\n\n@code\nint c;\n\n\nint d;\n@endcode\n");
    assertIncludes(out, "int a;\n\n\nint b;");
    assertIncludes(out, "int c;\n\n\nint d;");
  });

  test("@code keeps the indentation of a list item", async () => {
    const out = await convert("1. Step one:\n   @code{.sh}\n   make flash\n   @endcode\n2. Step two\n");
    assertIncludes(out, "1. Step one:\n   ```sh\n   make flash\n   ```\n2. Step two");
  });

  test("code spans are not transformed", async () => {
    const out = await convert("Use `\\ref boards_other` and `\\c foo` and `@note x` here.\n");
    assertIncludes(out, "Use `\\ref boards_other` and `\\c foo` and `@note x` here.");
  });

  test("a code span of a known symbol becomes a link", async () => {
    const out = await convert("Set `#GPIO_PIN` and #GPIO_PIN.\n");
    assertIncludes(out, `Set [\`#GPIO_PIN\`](${GPIO_PIN})`);
    assertIncludes(out, `and [\`GPIO_PIN\`](${GPIO_PIN}).`);
  });

  test("indented code blocks are not transformed", async () => {
    const out = await convert("Run:\n\n    #GPIO_PIN \\ref boards_other\n\nDone.\n");
    assert.match(out, /^ {4}#GPIO_PIN \\ref boards_other$/m);
  });

  test("indented text in a list is not code", async () => {
    const out = await convert("- item\n\n    see \\ref boards_other\n");
    assert.match(out, /^ {4}see \[Other Board\]\(\/boards\/other\/\)$/m);
  });

  test("a leading indented code block keeps its indentation", async () => {
    assert.equal(await convert("    make flash\n"), "    make flash\n");
  });

  test("without a tag file, groups are linked to their group page", async () => {
    const out = await convert("Uses \\ref drivers_periph_gpio and \\ref gpio_init.\n", { api: false });
    assertIncludes(out, `[drivers_periph_gpio](${GPIO_PAGE})`);
    assertIncludes(out, "and `gpio_init`.");
  });

  test("with a tag file, unknown names are not guessed", async () => {
    const out = await convert("Uses \\ref drivers_foo.\n");
    assertIncludes(out, "Uses `drivers_foo`.");
  });

  test("a rule in the first line is not front matter", async () => {
    // the Markdown processor removes `---` … `---` at the start of a document
    const out = await convert("@defgroup boards_g G\n@author A. Author\n\n---\n\nSome text.\n");
    assert.match(out, /^\*\*\*\n\nSome text\.\n\n---\n\n\*Author: A\. Author\*\n$/);
  });
});
