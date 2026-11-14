/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Conversion of the Doxygen flavoured Markdown used in `doc*.md` files of
 * boards and CPUs into plain CommonMark/GFM for Starlight.
 *
 * A file is parsed once with `parseDoxygenDoc()`. The conversion of the
 * parsed files works in two passes:
 *
 * 1. `collectDoxygenAnchors()` is called for every `doc*.md` file before any
 *    page is rendered. It records all anchors a file defines (`@defgroup`,
 *    `\section`, `\subsection`, `\anchor`, `{#id}` heading attributes) together
 *    with the URL of the page they end up on. The result is a global
 *    `AnchorIndex` that makes cross-page `\ref` resolution possible.
 *
 * 2. `transformDoxygenMarkdown()` converts a single file. It works block-wise
 *    rather than line-wise, because many Doxygen constructs span several lines
 *    (`\ref id "text"` with the text on the next line, `@note` paragraphs and
 *    lists, `@code` ... `@endcode`).
 *
 * Both passes need the ids of the headings. The caller renders
 * `headingMarkdown()` with the Markdown processor to get them.
 *
 * References to anything that is not rendered on this site (functions, macros,
 * other groups) are resolved with the tag file of the API documentation, see
 * `doxygen_tagfile.ts`. Without a tag file, references to groups are linked to
 * the group page their name implies, and everything else is rendered as plain
 * code.
 *
 * Anything inside code blocks and code spans is left untouched. Only a code
 * span of a known symbol (`` `#GPIO_PIN` ``, `` `gpio_init()` ``) becomes a
 * link as a whole.
 */

import { type ApiIndex, doxygenGroupFile } from "./doxygen_tagfile.ts";

/** Target of a Doxygen anchor. */
export interface AnchorTarget {
  /** Absolute site URL of the page including the fragment, e.g. `/cpus/esp32/#esp32_toc` */
  url: string;
  /** Human readable title used as link text if `\ref` has no explicit text */
  title: string;
}

/** True if `url` is the page `pageUrl` or an anchor on it. */
function isOnPage(url: string, pageUrl: string): boolean {
  return url === pageUrl || url.startsWith(`${pageUrl}#`);
}

/**
 * Global index of the Doxygen anchors and groups that are rendered on the
 * site, with their location.
 *
 * In Doxygen an id is unique, but nothing enforces that here: two boards can
 * both define `{#flashing}`. A reference resolves to the definition on its
 * own page, or else to the first definition.
 */
export class AnchorIndex {
  private readonly targets = new Map<string, AnchorTarget[]>();

  /** Record a definition of `id`. */
  add(id: string, target: AnchorTarget): void {
    const targets = this.targets.get(id);
    if (targets) {
      targets.push(target);
    } else {
      this.targets.set(id, [target]);
    }
  }

  /**
   * Resolve an id.
   *
   * @param id      Name of an anchor or group
   * @param pageUrl URL of the page that contains the reference, whose own
   *                definition of the id is preferred
   */
  lookup(id: string, pageUrl?: string): AnchorTarget | undefined {
    const targets = this.targets.get(id);
    return (
      (pageUrl !== undefined && targets?.find((target) => isOnPage(target.url, pageUrl))) ||
      targets?.[0]
    );
  }

  /** The ids that are defined more than once, with the URLs of their definitions. */
  duplicates(): [id: string, urls: string[]][] {
    return [...this.targets]
      .filter(([, targets]) => targets.length > 1)
      .map(([id, targets]) => [id, targets.map((target) => target.url)]);
  }
}

/** Context needed to transform one file. */
export interface TransformContext {
  /** Absolute URL of the page that is generated from the file, e.g. `/boards/esp32-wroom-32/` */
  pageUrl: string;
  /**
   * Repository relative path of the file, e.g. `boards/esp32-wroom-32/doc.md`.
   * A symbol that is defined by many boards or CPUs is resolved to the
   * definition that is nearest to this file.
   */
  sourcePath?: string;
  /** Global anchor index created by `collectDoxygenAnchors()` */
  anchors: AnchorIndex;
  /** Index of the API documentation, read from its Doxygen tag file */
  api?: ApiIndex;
  /** Base URL of the Doxygen API documentation, without a trailing `/` */
  apiBaseUrl: string;
  /** Ids of the headings of the page, from the Markdown processor */
  headingIds: string[];
  /** Callback for references that cannot be resolved (for build warnings) */
  onUnresolved?: (id: string) => void;
}

/* -------------------------------------------------------------------------- */
/* Helpers                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * Clean up text by removing leading/trailing asterisks and quotes.
 * Quotes are only removed if they enclose the whole text, a single trailing
 * `"` is an inch mark (e.g. `Rotary LCD 1.28"`).
 */
function stripAsterisksAndQuotes(value: string): string {
  return value
    .replace(/^\*+\s*/, "")
    .replace(/\*\/\s*$/, "")
    .trim()
    .replace(/^"(.*)"$/, "$1")
    .trim();
}

/** Length of the leading whitespace of a line. */
function indentOf(line: string): number {
  return line.length - line.trimStart().length;
}

/**
 * Remove the common indentation of all lines of text. The placeholder of a
 * code block (see `hideCodeBlocks()`) doesn't count, it loses up to the same
 * indentation. Lines of code blocks only are aligned by the code blocks.
 */
function dedent(lines: string[]): string[] {
  const filled = lines.filter((l) => l.trim());
  const text = filled.filter((l) => !l.includes("\uE002"));
  const indent = Math.min(...(text.length ? text : filled).map(indentOf));
  if (!Number.isFinite(indent) || indent === 0) {
    return lines;
  }
  const re = new RegExp(`^[ \\t]{0,${indent}}`);
  return lines.map((l) => l.replace(re, ""));
}

/** True if the end of `text` is inside a code span (an odd number of backticks). */
function isInCodeSpan(text: string): boolean {
  return (text.match(/`/g)?.length ?? 0) % 2 === 1;
}

/**
 * Width of the leading whitespace of a line. A tab reaches to the next
 * multiple of four.
 */
function indentWidth(line: string): number {
  let width = 0;
  for (const char of line) {
    if (char === " ") {
      width++;
    } else if (char === "\t") {
      width += 4 - (width % 4);
    } else {
      break;
    }
  }
  return width;
}

/**
 * A list item: `- `, `* `, `+ `, `1. ` or `1) `. The groups are the marker
 * with its indentation, and the whitespace behind the marker.
 */
const LIST_ITEM_RE = /^([ \t]*(?:[-*+]|\d+[.)]))(?:([ \t]+)|$)/;

/**
 * Indentation of the content of a list item (a match of `LIST_ITEM_RE`).
 * The lines that continue the item are indented this far.
 */
function listContentIndent(item: RegExpMatchArray): number {
  const space = item[2]?.length ?? 1;
  // more than four spaces behind the marker are not part of the indentation
  return indentWidth(item[1]) + item[1].trimStart().length + (space > 4 ? 1 : space);
}

/** The commands that start and end a `@code` block. */
const CODE_RE = /(?<![\w@\\])[@\\]code(?![\w])/;
const ENDCODE_RE = /[@\\]endcode(?![A-Za-z])/;

/**
 * Split a document into segments of prose and code so that the
 * transformations are never applied to code. Doxygen `@code` ... `@endcode`
 * blocks are converted to fenced code blocks here as well.
 *
 * Code is fenced code and indented code blocks. An indented code block
 * follows a blank line or a heading and is indented by four spaces. In a
 * list item the indentation counts from the content of the item:
 *
 *     1. Text of the item, its content starts at column 3
 *
 *            code, indented by 3 + 4 spaces
 *
 * A list item ends at a blank line that is followed by a line with less
 * indentation than the content of the item.
 */
function splitCode(content: string): { code: boolean; text: string }[] {
  const lines = content.split(/\r?\n/);
  const segments: { code: boolean; text: string }[] = [];
  let buffer: string[] = [];
  let fence: string | null = null;
  /** Indentation of the open `@code` block */
  let doxIndent = "";
  /** Indentation of the open indented code block, 0 if there is none */
  let indented = 0;
  /** Indentation of the content of the open list items, the innermost item is last */
  const listIndents: number[] = [];
  /** True after a blank line, a heading or a code block, where a code block can start */
  let afterBlank = true;

  const flush = (code: boolean) => {
    if (buffer.length) {
      segments.push({ code, text: buffer.join("\n") });
      buffer = [];
    }
  };
  /**
   * True if a `@code` block that is open in front of `lines[from]` ends:
   * there is an `@endcode`, and no other `@code` in front of it.
   */
  const hasEndcode = (from: number) => {
    for (let n = from; n < lines.length; n++) {
      if (ENDCODE_RE.test(lines[n])) {
        return true;
      }
      if (CODE_RE.test(lines[n])) {
        return false;
      }
    }
    return false;
  };

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    const blank = line.trim() === "";
    if (fence === "@endcode") {
      // `@endcode` may follow the last line of code (`};\endcode`), and text
      // may follow `@endcode`, which is continued as the next line
      const close = line.match(/^(.*?)[@\\]endcode(?![A-Za-z])[ \t]*(.*)$/);
      if (!close) {
        buffer.push(line);
        continue;
      }
      if (close[1].trim()) {
        buffer.push(close[1].trimEnd());
      }
      // The fence is longer than any fence in the code (`@code{.md}` with a
      // fenced code block), which would end the block otherwise.
      const inner = buffer.slice(1).map((l) => l.match(/^\s*(`{3,})/)?.[1].length ?? 0);
      const ticks = "`".repeat(Math.max(2, ...inner) + 1);
      buffer[0] = buffer[0].replace("```", ticks);
      buffer.push(`${doxIndent}${ticks}`);
      fence = null;
      flush(true);
      if (close[2]) {
        lines.splice(i + 1, 0, doxIndent + close[2]);
      }
      afterBlank = true;
      continue;
    }
    if (fence !== null) {
      if (
        line.trim().startsWith(fence[0].repeat(fence.length)) &&
        line.trim().replace(new RegExp(`^\\${fence[0]}+`), "") === ""
      ) {
        buffer.push(line);
        fence = null;
        flush(true);
      } else {
        buffer.push(line);
      }
      afterBlank = true;
      continue;
    }
    const width = indentWidth(line);
    if (indented) {
      if (blank || width >= indented) {
        buffer.push(line);
        continue;
      }
      // the blank lines at the end are not part of the code block
      const trailing: string[] = [];
      while (buffer.at(-1)?.trim() === "") {
        trailing.push(buffer.pop()!);
      }
      flush(true);
      buffer = trailing;
      indented = 0;
    }

    // The info string of a backtick fence must not contain backticks.
    // Otherwise the line is an inline code span like ```FOO``` and not the
    // start of a code block.
    const md = line.match(/^\s*(`{3,})[^`]*$/) ?? line.match(/^\s*(~{3,})/);
    if (md) {
      flush(false);
      fence = md[1];
      buffer.push(line);
      continue;
    }
    // `@code` is usually on a line of its own. It may also follow text
    // (`Signature:\code{.c}`), and the code may follow it on the same line
    // (`@code int x = 1; @endcode`), which is continued as the next line.
    // Without an `@endcode` of its own the command is text (`the \code
    // command`), and not a code block up to the end of the document or of
    // the next code block.
    const dox = line.match(
      /^(.*?)(?<![\w@\\])[@\\]code(?:\{\.?([^}\s`]*)\})?(?![\w{])[ \t]*(.*)$/,
    );
    if (dox && !isInCodeSpan(dox[1]) && (ENDCODE_RE.test(dox[3]) || hasEndcode(i + 1))) {
      const [, before, lang = "", rest] = dox;
      if (before.trim()) {
        buffer.push(before.trimEnd());
      }
      flush(false);
      fence = "@endcode";
      // The fence keeps the indentation of `@code`, so that a code block
      // inside a list item stays part of that list item.
      doxIndent = line.match(/^[ \t]*/)![0];
      buffer.push(`${doxIndent}\`\`\`${lang}`);
      if (rest) {
        lines.splice(i + 1, 0, doxIndent + rest);
      }
      continue;
    }
    // After a blank line, a line belongs to the innermost list item whose
    // content it is indented to. The items that are indented deeper end
    // here. A heading ends them without a blank line, too.
    const heading = /^ {0,3}#{1,6}[ \t]/.test(line);
    if (!blank && (afterBlank || heading)) {
      while (listIndents.length && listIndents.at(-1)! > width) {
        listIndents.pop();
      }
    }
    if (afterBlank && !blank) {
      const codeIndent = (listIndents.at(-1) ?? 0) + 4;
      if (width >= codeIndent) {
        flush(false);
        indented = codeIndent;
        buffer.push(line);
        continue;
      }
    }

    const item = line.match(LIST_ITEM_RE);
    if (item) {
      // an item ends the item before it and the items nested in that one
      while (listIndents.length && listIndents.at(-1)! > width) {
        listIndents.pop();
      }
      listIndents.push(listContentIndent(item));
    }
    afterBlank = blank || heading;
    buffer.push(line);
  }
  flush(fence !== null || indented > 0);
  return segments;
}

/**
 * Replace every code block of `content` with a placeholder line, so that the
 * transformations of the prose see where code is but not what it contains.
 * A code block that directly follows a `@note` line is part of the note
 * paragraph, as in Doxygen.
 *
 * `restore()` puts the code blocks back. A block is shifted by the change of
 * the indentation of its placeholder line (e.g. inside an aside).
 */
function hideCodeBlocks(content: string): { prose: string; restore: (text: string) => string } {
  const blocks: string[] = [];
  const prose = splitCode(content)
    .map((s) => (s.code ? `${s.text.match(/^[ \t]*/)![0]}\uE002${blocks.push(s.text) - 1}\uE003` : s.text))
    .join("\n");
  const reindent = (block: string, indent: number): string => {
    const delta = indent - indentOf(block);
    if (delta > 0) {
      return block.replace(/^(?=.*\S)/gm, " ".repeat(delta));
    }
    return delta < 0 ? block.replace(new RegExp(`^ {0,${-delta}}`, "gm"), "") : block;
  };
  const restore = (text: string) =>
    text
      .replace(/^([ \t]*)\uE002(\d+)\uE003[ \t]*$/gm, (_m, indent: string, n: string) =>
        reindent(blocks[Number(n)], indent.length),
      )
      .replace(/\uE002(\d+)\uE003/g, (_m, n: string) => blocks[Number(n)]);
  return { prose, restore };
}

/** A code span, `` `foo` `` or ``` ``a`b`` ```, within a line. */
const CODE_SPAN_RE = /(?<![`\\])(`+)(?!`)([^\n]*?[^`\n])\1(?!`)/g;

/** `text` with blanks in place of its code spans, to search outside of them. */
function blankCodeSpans(text: string): string {
  return text.replace(CODE_SPAN_RE, (span) => " ".repeat(span.length));
}

/**
 * Apply `fn` to `text` with all code spans hidden, so that nothing inside a
 * code span is transformed. The spans are replaced by placeholders of
 * private use characters, which no transformation matches.
 */
function mapOutsideCodeSpans(text: string, fn: (text: string) => string): string {
  const spans: string[] = [];
  const hidden = text.replace(CODE_SPAN_RE, (span) => `\uE000${spans.push(span) - 1}\uE001`);
  return fn(hidden).replace(/\uE000(\d+)\uE001/g, (_m, n: string) => spans[Number(n)]);
}

/* -------------------------------------------------------------------------- */
/* Group handling                                                             */
/* -------------------------------------------------------------------------- */

interface GroupBlock {
  /** Name of the Doxygen group, empty if the file doesn't define a group */
  name: string;
  title: string;
  brief?: string;
  authors: string[];
  /** Parent groups (`@ingroup`) */
  ingroups: string[];
  body: string;
}

/**
 * A line that starts with a Doxygen command or a Markdown heading, which
 * ends a paragraph. The commands that link or format a word (`\ref foo`,
 * `\c bar`) are part of a paragraph.
 */
const BLOCK_START_RE = /^\s*(?:[@\\](?!(?:ref|em|[abcep])\s)[A-Za-z{}]|#{1,6}\s)/;

/**
 * The lines of a document without its HTML comments, and whether a line is
 * code.
 *
 * A comment starts in prose, `<!--` in a code block or a code span is code.
 * A comment may contain code blocks, which are removed with it. Without a
 * `-->` in the prose that follows, `<!--` is text.
 */
function linesWithoutComments(content: string): { text: string; code: boolean }[] {
  const segments = splitCode(content);
  const lines: { text: string; code: boolean }[] = [];
  const push = (text: string, code: boolean) => {
    for (const line of text.split("\n")) {
      lines.push({ text: line, code });
    }
  };

  for (let n = 0; n < segments.length; n++) {
    const { code } = segments[n];
    if (code) {
      push(segments[n].text, true);
      continue;
    }
    const text = mapOutsideCodeSpans(segments[n].text, (prose) =>
      prose.replace(/<!--[\s\S]*?-->/g, ""),
    );
    const start = blankCodeSpans(text).indexOf("<!--");
    const end =
      start < 0
        ? -1
        : segments.findIndex((s, k) => k > n && !s.code && blankCodeSpans(s.text).includes("-->"));
    if (end < 0) {
      push(text, false);
      continue;
    }
    // the comment ends in a later prose segment, which is continued behind it
    push(text.slice(0, start), false);
    const rest = segments[end].text;
    segments[end] = { code: false, text: rest.slice(blankCodeSpans(rest).indexOf("-->") + 3) };
    n = end - 1;
  }
  return lines;
}

/**
 * Split a file at its `@defgroup` lines. Files like `cpu/esp32/doc.md` define
 * more than one group (e.g. `stdio_usb_serial_jtag` *and* `cpu_esp32`), and
 * only the groups with the site's prefix belong on the generated page.
 *
 * The group metadata (`@brief`, `@author`, `@ingroup`, …) and HTML comments
 * are removed from the body. Code blocks are kept as they are, also if they
 * contain lines that look like group metadata or HTML comments.
 *
 * A file without `@defgroup` results in a single block without a name.
 */
function splitGroups(content: string): GroupBlock[] {
  const lines = linesWithoutComments(content);
  const blocks: GroupBlock[] = [];
  const preamble: string[] = [];
  let block: GroupBlock | undefined;
  let body = preamble;
  /** `@ingroup` lines in front of the first `@defgroup` belong to it */
  let leadingIngroups: string[] = [];

  const finish = () => {
    if (block) {
      block.body = body.join("\n");
      blocks.push(block);
    }
  };

  for (let i = 0; i < lines.length; i++) {
    const { text, code } = lines[i];
    const command = code ? null : text.match(/^\s*[@\\](\w+|[{}])(?:\s+(.*))?$/);

    switch (command?.[1]) {
      case "defgroup": {
        finish();
        const [name, ...title] = (command[2] ?? "").trim().split(/\s+/);
        block = {
          name,
          // a title is plain text, `\ref foo` in a title shows `foo`
          title: stripAsterisksAndQuotes(title.join(" ").replace(/[@\\]ref\s+/g, "")),
          authors: [],
          ingroups: leadingIngroups,
          body: "",
        };
        leadingIngroups = [];
        body = [];
        break;
      }
      case "brief": {
        // the brief description is a paragraph, it ends at the next blank
        // line or at the next command that starts a block
        const brief = [command[2] ?? ""];
        while (
          i + 1 < lines.length &&
          !lines[i + 1].code &&
          lines[i + 1].text.trim() !== "" &&
          !BLOCK_START_RE.test(lines[i + 1].text)
        ) {
          brief.push(lines[++i].text.trim());
        }
        const value = stripAsterisksAndQuotes(brief.join(" ").trim());
        if (block && !block.brief) {
          block.brief = value || undefined;
        }
        break;
      }
      case "author":
      case "authors":
        // drop the e-mail address
        block?.authors.push((command[2] ?? "").replace(/<[^>]*@[^>]*>/, "").trim());
        break;
      case "ingroup":
        // `@ingroup a b` adds the group to several parents
        (block?.ingroups ?? leadingIngroups).push(
          ...(command[2] ?? "").trim().split(/\s+/).filter(Boolean),
        );
        break;
      case "addtogroup":
      case "name":
      case "{":
      case "}":
        break;
      default:
        body.push(text);
    }
  }
  finish();

  if (!blocks.length) {
    // no @defgroup, render the complete file
    blocks.push({
      name: "",
      title: "",
      authors: [],
      ingroups: leadingIngroups,
      body: preamble.join("\n"),
    });
  }
  return blocks;
}

/** Return the group blocks that belong to the site (matching `prefix_`). */
function siteGroups(blocks: GroupBlock[], prefix: string): GroupBlock[] {
  const own = blocks.filter((b) => b.name.startsWith(`${prefix}_`));
  return own.length ? own : blocks;
}

/* -------------------------------------------------------------------------- */
/* Sections and anchors                                                       */
/* -------------------------------------------------------------------------- */

const SECTION_RE =
  /^[ \t]*[@\\](section|subsection|subsubsection|paragraph)[ \t]+(\S+)[ \t]+(.+)$/gm;
/**
 * A heading, `## Title` or `## Title {#id}`. The groups are the `#` with the
 * whitespace behind them, the title, and the explicit id.
 */
const HEADING_RE = /^(#{1,6}[ \t]+)(.*?)[ \t]*(?:\{#([\w.:-]+)\})?[ \t]*$/gm;
/**
 * `\anchor id`, on a line of its own or inside a paragraph. The punctuation
 * of the sentence behind the id is not part of it (`see \anchor foo.`).
 */
const ANCHOR_RE = /(?<![\w@\\])[@\\]anchor[ \t]+([\w.:-]*\w)/g;

/**
 * The `\section` at the very beginning of a document repeats the page title.
 * It is dropped from the page, and its anchor points to the top of the page.
 */
const TITLE_SECTION_RE = /^\s*[@\\]section[ \t]+(\S+)[ \t]+(.+)$/m;

/** Match of the title section, if the document starts with one. */
function titleSection(body: string): RegExpMatchArray | null {
  const match = body.match(TITLE_SECTION_RE);
  return match && body.slice(0, match.index).trim() === "" ? match : null;
}

interface PagePart {
  block: GroupBlock;
  /** Body of the group as it is rendered on the page */
  body: string;
  /** Id of the dropped title section */
  titleSectionId?: string;
  /** Title of the dropped title section, used as link text of `\ref` */
  titleSectionTitle?: string;
}

/**
 * The parts of a page: the first group is the page itself, all further groups
 * of the same file (e.g. `cpu_efm32_*` in `cpu/efm32/doc.md`) are appended as
 * sections with the group name as anchor.
 */
function pageParts(blocks: GroupBlock[]): PagePart[] {
  return blocks.map((block, n) => {
    if (n > 0) {
      return {
        block,
        body: `\n# ${block.title} {#${block.name}}\n\n${block.brief ?? ""}\n${block.body}`,
      };
    }
    const title = titleSection(block.body);
    return {
      block,
      body: title ? block.body.replace(TITLE_SECTION_RE, "") : block.body,
      titleSectionId: title?.[1],
      titleSectionTitle: title ? stripAsterisksAndQuotes(title[2]) : undefined,
    };
  });
}

/** A parsed `doc*.md` file, see `parseDoxygenDoc()`. */
export interface DoxygenDoc {
  /** Title of the page from its `@defgroup`, empty if the file has no group */
  title: string;
  /**
   * Description of the page, the `@brief` of its own group (not the first
   * `@brief` in the file, which may belong to another group)
   */
  brief?: string;
  /** Names of the Doxygen groups that are rendered on the page */
  groups: string[];
  /** Parent groups (`@ingroup`) of the groups that are rendered on the page */
  ingroups: string[];
  /**
   * True if the groups have a body, i.e. more than `@defgroup`, `@brief` and
   * a title section. Groups without a body (e.g. most of `boards/common/*`)
   * only collect header files, their API documentation page is more useful
   * than a page with a title only.
   */
  hasContent: boolean;
  /** The parts of the page, input of both passes of the conversion */
  parts: PagePart[];
}

/**
 * Parse a `doc*.md` file. The result has the metadata of the page, and it is
 * the input of `collectDoxygenAnchors()` and `transformDoxygenMarkdown()`.
 *
 * @param content     Content of the file
 * @param groupPrefix Prefix of the Doxygen groups rendered on this site, e.g.
 *                    `boards` or `cpu`
 */
export function parseDoxygenDoc(content: string, groupPrefix: string): DoxygenDoc {
  const parts = pageParts(siteGroups(splitGroups(content), groupPrefix));
  const blocks = parts.map((part) => part.block);
  return {
    title: blocks[0].title,
    // a description is plain text, `\ref foo` in it shows `foo`
    brief: blocks[0].brief?.replace(/[@\\]ref\s+/g, ""),
    groups: blocks.map((block) => block.name).filter(Boolean),
    ingroups: [...new Set(blocks.flatMap((block) => block.ingroups))],
    hasContent: parts.some((part, n) => (n === 0 ? part.body : part.block.body).trim() !== ""),
    parts,
  };
}

/**
 * The body of a page with its code blocks hidden, and with all headings as
 * `## Title {#id}`.
 *
 * Both passes start from this text, so that the anchor index always matches
 * the generated page.
 */
function pageProse(doc: DoxygenDoc): { prose: string; restore: (text: string) => string } {
  const { prose, restore } = hideCodeBlocks(doc.parts.map((part) => part.body).join("\n"));
  return { prose: transformSetextHeadings(transformSections(prose)), restore };
}

interface PageHeadings {
  /** The ids that the Markdown processor gives the headings of the page */
  slugs: Set<string>;
  /** The explicit heading ids (`{#id}`) and the id of the anchor of each */
  anchorIds: Map<string, string>;
}

/**
 * The ids of the headings of a page (the text of `pageProse()`).
 *
 * The Markdown processor gives every heading the slug of its text as id. An
 * explicit id that is the slug of another heading would be on the page twice:
 *
 *     ## Flashing             id "flashing" (slug)
 *     ## Flashing {#flashing} id "flashing-1" (slug), anchor "flashing"
 *
 * The slugs can't be changed from here, so the anchor of such an id gets a
 * free id instead (`flashing-2`). All references use the anchor index, which
 * has the id of the anchor, so they still link to the heading with the
 * explicit id.
 */
function pageHeadings(prose: string, headingIds: string[], pageUrl: string): PageHeadings {
  const matches = [...prose.matchAll(HEADING_RE)];
  if (matches.length !== headingIds.length) {
    throw new Error(`${pageUrl}: ${headingIds.length} heading ids for ${matches.length} headings`);
  }
  const headings = matches.map((m, n) => ({ slug: headingIds[n], id: m[3] }));
  const slugs = new Set(headings.map((heading) => heading.slug));
  const used = new Set(slugs);
  for (const { id } of headings) {
    if (id) {
      used.add(id);
    }
  }
  const anchorIds = new Map<string, string>();
  for (const { slug, id } of headings) {
    if (!id || anchorIds.has(id)) {
      continue;
    }
    let anchorId = id;
    if (id !== slug && slugs.has(id)) {
      for (let n = 1; used.has(anchorId); n++) {
        anchorId = `${id}-${n}`;
      }
      used.add(anchorId);
    }
    anchorIds.set(id, anchorId);
  }
  return { slugs, anchorIds };
}

/**
 * Record all anchors defined in a file. Must be called for every file before
 * the first call of `transformDoxygenMarkdown()`.
 *
 * Only the groups that are rendered on the site are indexed. References to
 * anchors of other groups (e.g. `stdio_usb_serial_jtag` in `cpu/esp32/doc.md`)
 * are resolved with the tag file of the API documentation.
 *
 * @param headingIds Ids of the headings. Not needed for the index of
 *                   `headingMarkdown()`.
 */
export function collectDoxygenAnchors(
  doc: DoxygenDoc,
  pageUrl: string,
  index: AnchorIndex,
  headingIds?: string[],
): void {
  // The first group is the page. Further groups are indexed below by the
  // `{#name}` heading that `pageParts()` adds for them.
  const { block, titleSectionId, titleSectionTitle } = doc.parts[0];
  if (block.name) {
    index.add(block.name, { url: pageUrl, title: block.title || block.name });
  }
  // The title section is dropped, its anchor points to the top of the page.
  // As in Doxygen, a `\ref` without text shows the title of the section.
  if (titleSectionId) {
    index.add(titleSectionId, { url: pageUrl, title: titleSectionTitle || block.title });
  }

  const { prose } = pageProse(doc);
  const anchorIds = headingIds && pageHeadings(prose, headingIds, pageUrl).anchorIds;
  for (const [, , title, id] of prose.matchAll(HEADING_RE)) {
    if (id) {
      index.add(id, { url: `${pageUrl}#${anchorIds?.get(id) ?? id}`, title: title.trim() });
    }
  }
  for (const [, id] of prose.matchAll(ANCHOR_RE)) {
    index.add(id, { url: `${pageUrl}#${id}`, title: id });
  }
}

/* -------------------------------------------------------------------------- */
/* Pass 2: transformation                                                     */
/* -------------------------------------------------------------------------- */

/** Make a URL relative to the current page if it points to the same page. */
function localize(url: string, ctx: TransformContext): string {
  return url.startsWith(`${ctx.pageUrl}#`) ? url.slice(ctx.pageUrl.length) : url;
}

interface ResolvedRef {
  url: string;
  /** Link text if the reference has no explicit text */
  title: string;
  /** True if the title is a code symbol */
  code: boolean;
}

/**
 * Names of groups of the API documentation (`drivers_periph_gpio`,
 * `boards_common_nucleo`). Functions, types and section anchors don't use
 * these prefixes.
 */
const API_GROUP_RE = /^(?:boards|cpu|drivers|net|pkg|sys)_[a-z0-9_-]+$/;

/**
 * Resolve a `\ref` target. Order:
 * 1. anchors and groups rendered on this site (global anchor index)
 * 2. the API documentation (tag file)
 * 3. without a tag file only: the group page implied by a group name
 *
 * Anything else is unresolved. Other names are not guessed, because a wrong
 * guess results in a dead link.
 */
function resolveRef(id: string, ctx: TransformContext): ResolvedRef | undefined {
  const target = lookupRef(id, ctx);
  if (!target) {
    ctx.onUnresolved?.(id);
  }
  return target;
}

/** `resolveRef()` without reporting unresolved references. */
function lookupRef(id: string, ctx: TransformContext): ResolvedRef | undefined {
  const local = ctx.anchors.lookup(id, ctx.pageUrl);
  if (local) {
    return { url: localize(local.url, ctx), title: local.title, code: false };
  }
  if (!ctx.api) {
    return API_GROUP_RE.test(id)
      ? { url: `${ctx.apiBaseUrl}/${doxygenGroupFile(id)}`, title: id, code: false }
      : undefined;
  }
  const api = ctx.api.lookup(id.replace(/\(\)$/, ""), ctx.sourcePath);
  return api
    ? { url: `${ctx.apiBaseUrl}/${api.url}`, title: api.title ?? id, code: api.symbol }
    : undefined;
}

/**
 * `\ref id "text"`, `@ref id`, also with the text on the next line, and
 * Markdown links with a reference as target, `[text](@ref id)`.
 * Unresolved references are rendered as their text or as code.
 */
function transformRefs(text: string, ctx: TransformContext): string {
  // the link text may contain brackets, e.g. an image `[![alt](img.png)](@ref id)`
  const withLinks = text.replace(
    /\[((?:[^[\]]|\[[^[\]]*\])*)\]\(\s*[@\\]ref\s+([^\s)]+)\s*\)/g,
    (_m, label: string, id: string) => {
      const target = resolveRef(id, ctx);
      return target ? `[${label}](${target.url})` : label;
    },
  );
  return withLinks.replace(
    /(?<![\w@\\])[@\\]ref\s+([A-Za-z0-9_.:#~-]*[A-Za-z0-9_](?:\(\))?)(?:[ \t]*\n?[ \t]*"([^"\n]+)")?/g,
    (_m, id: string, label?: string) => {
      const target = resolveRef(id, ctx);
      if (!target) {
        return label ?? `\`${id}\``;
      }
      const linkText = label ?? (target.code ? `\`${target.title}\`` : target.title);
      return `[${linkText}](${target.url})`;
    },
  );
}

/**
 * Markdown links to `#anchor` that is defined on another page (e.g. links
 * from `doc_esp32.doc.md` to sections of `doc.md`) are rewritten to absolute
 * links.
 *
 * A link to a heading of the page itself stays as it is: to its explicit id,
 * or to the id that the Markdown processor gives the heading (`slugs`), also
 * if another page defines an anchor of the same name. A link to an anchor
 * that doesn't exist is reported as unresolved.
 */
function transformFragmentLinks(text: string, ctx: TransformContext, slugs: Set<string>): string {
  return text.replace(/\]\(#([\w.:-]+)\)/g, (m, id: string) => {
    const target = ctx.anchors.lookup(id, ctx.pageUrl);
    if (slugs.has(id) && !(target && isOnPage(target.url, ctx.pageUrl))) {
      return m;
    }
    if (!target) {
      ctx.onUnresolved?.(`#${id}`);
      return m;
    }
    return `](${localize(target.url, ctx)})`;
  });
}

/**
 * The references in the label of a Markdown link (`[see \ref foo](url)`,
 * `[the #FOO macro](url)`) are converted to links like all others. But a link
 * can't contain a link, the Markdown processor shows the brackets and the URL
 * of the outer link as text then. The outer link is kept, and a link in its
 * label is replaced by the label of that link. An image in a label stays.
 */
function unnestLinks(text: string): string {
  return text.replace(/\[((?:[^[\]]|\[[^[\]]*\])*)\](?=\()/g, (_m, label: string) => {
    return `[${label.replace(/(?<!!)\[([^[\]]*)\]\([^()]*\)/g, "$1")}]`;
  });
}

/**
 * Setext headings (a line of text, underlined with `===` or `---`) are
 * converted to ATX headings, so that `shiftHeadings()` and the TOC handle
 * them like all other headings. Only a single line paragraph is converted,
 * as Doxygen does.
 */
function transformSetextHeadings(text: string): string {
  const lines = text.split("\n");
  for (let i = 1; i < lines.length; i++) {
    const underline = lines[i].match(/^ {0,3}(=+|-+)[ \t]*$/);
    const title = lines[i - 1];
    if (!underline || !title.trim() || (i > 1 && lines[i - 2].trim() !== "")) {
      continue;
    }
    // the line before is not a paragraph: indented code, list item, table
    // row, heading, quote, HTML, Doxygen command, code block
    if (
      /^ {4}|^\s*(?:[-*+][ \t]|\d+[.)][ \t]|\||#|>|<|[@\\]\w|:::)/.test(title) ||
      /[|\uE002]/.test(title)
    ) {
      continue;
    }
    const indent = title.match(/^\s*/)![0];
    lines[i - 1] = `${indent}${underline[1][0] === "=" ? "#" : "##"} ${title.trim()}`;
    lines[i] = "";
  }
  return lines.join("\n");
}

/** Doxygen sections become headings with an explicit anchor. */
function transformSections(text: string): string {
  return text.replace(SECTION_RE, (_m, kind: string, id: string, title: string) => {
    const level = { section: 1, subsection: 2, subsubsection: 3, paragraph: 4 }[kind]!;
    return `${"#".repeat(level)} ${stripAsterisksAndQuotes(title)} {#${id}}`;
  });
}

/**
 * Shift the heading levels so that the top-most heading of the document is
 * `h2`. The page title rendered by Starlight is the only `h1`, and Starlight's
 * "On this page" TOC only contains `h2`..`h<maxHeadingLevel>`, which is why
 * all level-1 headings of `cpu/esp32/doc.md` are missing in the TOC.
 */
function shiftHeadings(text: string): string {
  const levels = [...text.matchAll(/^(#{1,6})[ \t]/gm)].map((m) => m[1].length);
  if (!levels.length) {
    return text;
  }
  const shift = Math.max(0, 2 - Math.min(...levels));
  if (!shift) {
    return text;
  }
  return text.replace(/^(#{1,6})([ \t])/gm, (_m, hashes: string, ws: string) =>
    "#".repeat(Math.min(6, hashes.length + shift)) + ws,
  );
}

/**
 * Explicit heading ids `{#id}`. Doxygen uses them as link targets, so they
 * have to survive. They are rendered as an empty anchor inside the heading,
 * which works with any Markdown processor and keeps the slug based ids that
 * Starlight uses for its TOC. If a processor plugin for heading attributes
 * (e.g. `satteri-custom-header-id`) is used instead, this function can simply
 * return the text unchanged.
 *
 * The anchor is put in front of the heading text. Behind the text, the space
 * before the anchor becomes part of the slug (`#overview-`) and of the TOC
 * entry.
 *
 * An id is unique on a page. If the Markdown processor gives the heading
 * this id already (`# Setting a timezone {#setting-a-timezone}`), no anchor
 * is added. Only the first heading with an id gets its anchor. An id that
 * the processor gives another heading is replaced, see `pageHeadings()`.
 *
 * @param anchorIds  The explicit ids and the id of the anchor of each, from
 *                   `pageHeadings()`
 * @param headingIds Ids of the headings, from the Markdown processor
 */
function transformHeadingIds(
  text: string,
  anchorIds: Map<string, string>,
  headingIds: string[],
): string {
  const done = new Set<string>();
  let n = 0;
  return text.replace(HEADING_RE, (m, hashes: string, heading: string, id?: string) => {
    const slug = headingIds[n++];
    if (!id) {
      return m;
    }
    const anchorId = anchorIds.get(id) ?? id;
    const anchor = done.has(id) || anchorId === slug ? "" : `<a id="${anchorId}"></a>`;
    done.add(id);
    return `${hashes}${anchor}${heading}`;
  });
}

/** `\anchor id` → empty HTML anchor */
function transformAnchors(text: string): string {
  return text.replace(ANCHOR_RE, (_m, id: string) => `<a id="${id}"></a>`);
}

/** A Markdown link `[label](target)`, as a group for `String.split()`. */
const MARKDOWN_LINK_RE = /(\[(?:[^[\]]|\[[^[\]]*\])*\]\([^)]*\))/;

const CALLOUTS: Record<string, { type: string; title?: string }> = {
  note: { type: "note" },
  remark: { type: "note" },
  remarks: { type: "note" },
  see: { type: "note", title: "See also" },
  sa: { type: "note", title: "See also" },
  tip: { type: "tip" },
  hint: { type: "tip" },
  warning: { type: "caution" },
  attention: { type: "caution" },
  important: { type: "danger" },
  experimental: { type: "caution", title: "Experimental" },
};

/** Callout commands that are another name of a command. */
const CALLOUT_ALIASES: Record<string, string> = { sa: "see", remarks: "remark" };

/** The kind of a callout command, `@sa` is the same kind as `@see`. */
function calloutKind(command: string): string {
  const name = command.toLowerCase();
  return CALLOUT_ALIASES[name] ?? name;
}

/** True if `lines[i]` starts a heading or a table, which ends a paragraph. */
function startsBlock(lines: string[], i: number): boolean {
  return (
    /^\s*#{1,6}[ \t]/.test(lines[i]) ||
    (lines[i].includes("|") && TABLE_SEPARATOR_RE.test(lines[i + 1] ?? ""))
  );
}

/**
 * `@note`, `@warning`, … paragraphs. In Doxygen such a paragraph ends at the
 * next blank line, the next paragraph command, or a heading or table. In a
 * list item it also ends at the next item of the list. Both forms are used:
 *
 *     @note Text that continues
 *           on the next lines.
 *
 *     @note
 *     - list item
 *     - list item
 *
 * They are converted to Starlight asides (`:::note` … `:::`). The aside keeps
 * the indentation of the command, so that a callout inside a list item stays
 * part of that list item. A code block directly below a line of the paragraph
 * is part of the paragraph. Consecutive paragraphs of the same kind (`@note`
 * directly after `@note`) are joined into one aside.
 *
 * `@see` and `@sa` take references without `\ref` (`@see boards_common_foo`).
 * Doxygen links them automatically, here they are converted to `\ref`, which
 * is resolved by `transformRefs()`.
 */
function transformCallouts(text: string, ctx: TransformContext): string {
  const lines = text.split("\n");
  const out: string[] = [];
  const startRe = new RegExp(
    `^(\\s*)[@\\\\](${Object.keys(CALLOUTS).join("|")})\\b[ \\t]*(.*)$`,
    "i",
  );

  for (let i = 0; i < lines.length; i++) {
    const start = lines[i].match(startRe);
    if (!start) {
      out.push(lines[i]);
      continue;
    }
    const [, indent, command] = start;
    const kind = calloutKind(command);
    const callout = CALLOUTS[command.toLowerCase()];

    // Consecutive paragraphs of the same kind are joined into one aside, as
    // Doxygen does.
    const body: string[] = [];
    for (let paragraph: RegExpMatchArray | null = start; paragraph; ) {
      // collect the paragraph: everything up to the next blank line, the
      // next paragraph command, heading or table
      const continuation: string[] = [];
      while (
        i + 1 < lines.length &&
        lines[i + 1].trim() !== "" &&
        !startRe.test(lines[i + 1]) &&
        !startsBlock(lines, i + 1) &&
        // the next item of the list that the command is in
        !(LIST_ITEM_RE.test(lines[i + 1]) && indentOf(lines[i + 1]) < indent.length)
      ) {
        continuation.push(lines[++i]);
      }
      // Continuation lines are indented to align with the text of the
      // command. The common indentation is removed so that they don't become
      // code blocks.
      const paragraphLines = dedent(continuation);
      const inline = paragraph[3].trim();
      if (inline) {
        paragraphLines.unshift(inline);
      }
      if (kind === "experimental" && !paragraphLines.length) {
        paragraphLines.push("This feature is experimental!");
      }
      body.push(...(body.length ? [""] : []), ...paragraphLines);

      const next = lines[i + 1]?.match(startRe);
      paragraph = null;
      if (next && calloutKind(next[2]) === kind) {
        paragraph = next;
        i++;
      }
    }
    if (callout.title === "See also") {
      // a bare name like `boards_common_foo`, `foo()` or `foo_t::bar`,
      // separated by whitespace or commas (not part of a URL, and not
      // already the target of `\ref`)
      const linkNames = (text: string) =>
        text.replace(
          /(^|[\s,])(?<![@\\]ref\s+)([A-Za-z_][\w-]*(?:::\w+)?(?:\(\))?)(?=[\s,;]|\.(?:\s|$)|$)/g,
          (m, before: string, id: string) =>
            /[_:(]/.test(id) && lookupRef(id, ctx) !== undefined ? `${before}@ref ${id}` : m,
        );
      for (let n = 0; n < body.length; n++) {
        // a Markdown link stays as it is, a name in its label is text
        body[n] = body[n]
          .split(MARKDOWN_LINK_RE)
          .map((piece, k) => (k % 2 ? piece : linkNames(piece)))
          .join("");
      }
    }
    out.push(
      "",
      `${indent}:::${callout.type}${callout.title ? `[${callout.title}]` : ""}`,
      ...body.map((l) => indent + l),
      `${indent}:::`,
      "",
    );
  }
  return out.join("\n");
}

/**
 * HTML blocks such as `<center>` are a problem in CommonMark: an HTML block
 * starting with `<center>` only ends at the next blank line, so a table that
 * directly follows `<center>` is emitted as raw text. Doxygen doesn't have this
 * problem. The tags are only used to center tables, so they are removed.
 *
 * A line with only `<br>` has the same problem: the paragraph that follows is
 * not parsed as Markdown, and directly after a table it becomes an empty
 * table row. The line is only used as vertical space, so it is replaced by an
 * empty line.
 */
function transformHtmlBlocks(text: string): string {
  return text
    .replace(/<\/?center>/gi, "")
    .replace(/^[ \t]*(?:<br\s*\/?>[ \t]*)+$/gim, "");
}

/**
 * The delimiter row below the header row of a table, `|---|:--:|`, `-|-` or
 * `|:-|` (one column). A cell has at least one `-`.
 */
const TABLE_SEPARATOR_RE =
  /^\s*(?:\|?\s*:?-+:?\s*(?:\|\s*:?-+:?\s*)+\|?|\|\s*:?-+:?\s*\|)\s*$/;

/**
 * Doxygen accepts table rows that are indented by four or more spaces
 * relative to the header row (e.g. right aligned numbers in the first column),
 * some Markdown parsers end the table there. All rows of a table are therefore
 * aligned with its header row. The indentation of the header row is kept,
 * because a table inside a list item has to stay indented.
 *
 * In Doxygen a table ends at the first line without `|`. In GFM such a line
 * still belongs to the table (lazy continuation) and becomes a row, so an
 * empty line is inserted in front of it.
 *
 * A `|` ends a cell, also directly behind the word of `\c`, `\b`, …
 * (`| \c FOO| bar |`). Outside of a table it is part of the word, see
 * `transformInlineCommands()`. A space is inserted in front of it, so that
 * the word ends at the cell.
 */
function transformTables(text: string): string {
  const endWordsAtCells = (row: string) =>
    row.replace(/([@\\](?:em|[abcep])[ \t]+(?:\\.|[^\s|\\])+)\|/g, "$1 |");
  const lines = text.split("\n");
  let indent: string | null = null;
  for (let i = 1; i < lines.length; i++) {
    if (TABLE_SEPARATOR_RE.test(lines[i])) {
      indent = lines[i - 1].match(/^\s*/)![0];
      lines[i - 1] = endWordsAtCells(lines[i - 1]);
    } else if (indent === null || lines[i].trim() === "") {
      indent = null;
      continue;
    } else if (!lines[i].includes("|")) {
      lines[i] = `\n${lines[i]}`;
      indent = null;
      continue;
    }
    lines[i] = indent + endWordsAtCells(lines[i].trimStart());
  }
  return lines.join("\n");
}

/**
 * The word argument of `\c`, `\b`, … runs to the next whitespace. Sentence
 * punctuation and an unbalanced closing parenthesis at its end are not part
 * of it (`\c FOO.`, `(see \c FOO)`), but a single character is a word.
 *
 * @returns the word and the text that follows it
 */
function splitWord(word: string): [string, string] {
  const count = (text: string, char: string) => text.split(char).length - 1;
  let end = word.length;
  while (end > 1) {
    const head = word.slice(0, end);
    if (/[.,;:!?]$/.test(head) || (head.endsWith(")") && count(head, "(") < count(head, ")"))) {
      end--;
    } else {
      break;
    }
  }
  return [word.slice(0, end), word.slice(end)];
}

/** Commands that format a word, and their Markdown */
const WORD_COMMANDS: Record<string, (word: string) => string> = {
  c: (word) => (word.includes("`") ? `\`\` ${word} \`\`` : `\`${word}\``),
  b: (word) => `**${word.replaceAll("*", "\\*")}**`,
  e: (word) => `*${word.replaceAll("*", "\\*")}*`,
};
WORD_COMMANDS.p = WORD_COMMANDS.c;
WORD_COMMANDS.em = WORD_COMMANDS.a = WORD_COMMANDS.e;

/**
 * Inline commands: `\c`, `\p` (code), `\b` (bold), `\e`, `\em`, `\a`
 * (emphasis) and `@details`. A command glued to a word (`user@p`) is text.
 */
function transformInlineCommands(text: string): string {
  return text
    .replace(/^([ \t]*)[@\\]details\b[ \t]*/gm, "$1")
    .replace(
      /(?<![\w@\\])[@\\](em|[cpbea])(?=\s)[ \t]*\n?[ \t]*([^\s]+)/g,
      (_m, command: string, argument: string) => {
        const [word, rest] = splitWord(argument);
        return WORD_COMMANDS[command](word) + rest;
      },
    );
}

/** Argument list of a call: `()`, `(0)` or `(PORT_A, 3)`. */
const CALL_ARGS = String.raw`\((?:[\w,:*&[\] \t]|\.\.\.)*\)`;

/**
 * Doxygen auto-links symbols prefixed by `#` (e.g. `#PWM_CH_NUMOF_MAX`,
 * `#adc_init`). In prose the `#` is only the link marker and is not shown.
 *
 * They are linked to the API documentation if the tag file knows them.
 * Otherwise an upper case name (a macro) is rendered as code, and any other
 * name is left unchanged, because `#define`, `#include` or `#1` are not
 * references. The target of a Markdown link `[text](#ID)` is not a symbol.
 *
 * The arguments of a macro are part of the link (`#PWM_DEV(0)`).
 *
 * Inside a code span (`` `#SPI_DEV(0)` ``) Doxygen links the symbol, too, but
 * keeps the `#`, see `transformSymbolCodeSpans()`.
 */
function transformSymbolLinks(text: string, ctx: TransformContext): string {
  const isMacro = (symbol: string) => /^[A-Z][A-Z0-9_]{2,}$/.test(symbol);
  return text.replace(
    new RegExp(String.raw`(?<=^|[\s(])(?<!\]\()#([A-Za-z_]\w*)(${CALL_ARGS})?(?![\w-])`, "gm"),
    (m, symbol: string, args = "") => {
      const url = apiSymbolUrl(symbol, ctx);
      if (!url) {
        return isMacro(symbol) ? `\`${symbol}${args}\`` : m;
      }
      // function arguments stay text
      const linked = apiCallUrl(symbol, args, ctx) ? args : "";
      return `[\`${symbol}${linked}\`](${url})${args.slice(linked.length)}`;
    },
  );
}

/**
 * A call in prose, not inside a word, path, URL or quoted string. Code
 * spans and HTML tags are matched first, so that nothing in them is a call.
 */
const CALL_RE = new RegExp(
  String.raw`${CODE_SPAN_RE.source}|<[A-Za-z/][^<>\n]*>|` +
    String.raw`(?<![\w$#@\\.:/~+=&?%"'-])(?<name>[A-Za-z_]\w*)(?<args>${CALL_ARGS})`,
  "g",
);

/**
 * Doxygen also auto-links calls without `#` (`gpio_init()`, `SPI_DEV(0)`).
 * Unknown calls are left unchanged.
 */
function transformCallLinks(text: string, ctx: TransformContext): string {
  return text.replace(CALL_RE, (m, ...groups) => {
    const { name, args } = groups.at(-1) as { name?: string; args?: string };
    const url = name && apiCallUrl(name, args!, ctx);
    return url ? `[\`${m}\`](${url})` : m;
  });
}

/**
 * A code span with a `#` symbol (`` `#SPI_DEV(0)` ``) or a call
 * (`` `gpio_init()` ``) is linked as a whole if the tag file knows the
 * symbol. Unknown symbols in code spans are left unchanged.
 */
function transformSymbolCodeSpans(text: string, ctx: TransformContext): string {
  return text.replace(
    /(?<!`)`(#?)([A-Za-z_]\w*)((?:\([^`()]*\))?)`(?!`)/g,
    (m, hash: string, symbol: string, args: string) => {
      // without `#`, only a call is linked
      const url = hash ? apiSymbolUrl(symbol, ctx) : args && apiCallUrl(symbol, args, ctx);
      return url ? `[${m}](${url})` : m;
    },
  );
}

/** URL of a code symbol in the API documentation. */
function apiSymbolUrl(symbol: string, ctx: TransformContext): string | undefined {
  const api = ctx.api?.lookup(symbol, ctx.sourcePath);
  return api?.symbol ? `${ctx.apiBaseUrl}/${api.url}` : undefined;
}

/**
 * URL of a symbol that is written as a call. As in Doxygen, `()` matches
 * any symbol, an argument list only a macro (`SPI_DEV(0)`).
 */
function apiCallUrl(symbol: string, args: string, ctx: TransformContext): string | undefined {
  const api = ctx.api?.lookup(symbol, ctx.sourcePath);
  return api?.symbol && (api.macro || args === "()") ? `${ctx.apiBaseUrl}/${api.url}` : undefined;
}

// Images

const IMAGE_BASE_PATH = "/img";
const IMAGE_EXTENSION_PATTERN = /\.(?:svg|png|jpe?g|gif|webp|avif)$/i;
const ABSOLUTE_URL_PATTERN = /^(?:[A-Za-z][A-Za-z0-9+.-]*:|\/\/|\/|#)/;

function rewriteImageReference(target: string): string {
  if (ABSOLUTE_URL_PATTERN.test(target) || !IMAGE_EXTENSION_PATTERN.test(target)) {
    return target;
  }
  return `${IMAGE_BASE_PATH}/${target.split("/").pop()}`;
}

function transformImages(text: string): string {
  return text
    // `@image <format> <file> ["caption"] [<size>=<value>]`, only the image
    // for HTML is shown
    .replace(/^[ \t]*[@\\]image[ \t]+(\w+)[ \t]+(\S+)(?:[ \t]+(.+))?$/gm, (_m, format: string, file: string, args = "") => {
      if (format !== "html") {
        return "";
      }
      const caption =
        args.match(/"([^"]+)"/)?.[1] ||
        args.replace(/\s*[A-Za-z0-9_-]+\s*=\s*\S+/g, "").trim() ||
        "Image";
      return `![${stripAsterisksAndQuotes(caption)}](${file})`;
    })
    .replace(
      /(<img\b[^>]*?\ssrc\s*=\s*(["']))(.*?)\2/gi,
      (_m, prefix: string, quote: string, target: string) =>
        `${prefix}${rewriteImageReference(target)}${quote}`,
    )
    .replace(
      /(!\[[^\]]*\]\(\s*)([^)\s]+)/g,
      (_m, prefix: string, target: string) => `${prefix}${rewriteImageReference(target)}`,
    );
}

// Main entry

/** Convert the text of `pageProse()`. The headings stay as `## Title {#id}`. */
function transformProse(prose: string, ctx: TransformContext, slugs: Set<string>): string {
  const result = mapOutsideCodeSpans(transformSymbolCodeSpans(prose, ctx), (text) => {
    text = transformHtmlBlocks(text);
    text = transformTables(text);
    text = transformCallouts(text, ctx);
    // before `transformRefs()`, whose links are not looked up again
    text = transformFragmentLinks(text, ctx, slugs);
    text = transformRefs(text, ctx);
    text = transformAnchors(text);
    text = transformInlineCommands(text);
    text = transformSymbolLinks(text, ctx);
    text = transformCallLinks(text, ctx);
    text = transformImages(text);
    text = unnestLinks(text);
    return text;
  });
  // heading operations need the complete document (levels are relative)
  return shiftHeadings(result);
}

/**
 * The converted headings of a page, to render them for their ids:
 * `## Pins of \ref foo {#pins}` → `## Pins of [Foo](/boards/foo/)`.
 */
export function headingMarkdown(
  doc: DoxygenDoc,
  ctx: Omit<TransformContext, "headingIds">,
): string {
  const draft = { ...ctx, headingIds: [], onUnresolved: undefined };
  const text = transformProse(pageProse(doc).prose, draft, new Set());
  return [...text.matchAll(HEADING_RE)].map((m) => m[1] + m[2]).join("\n\n");
}

/**
 * Transform the Doxygen Markdown of a `doc*.md` file into Markdown for the
 * generated page.
 */
export function transformDoxygenMarkdown(doc: DoxygenDoc, ctx: TransformContext): string {
  const authors = [...new Set(doc.parts.flatMap((p) => p.block.authors).filter(Boolean))];

  // The code blocks stay hidden up to the end, so that no step changes them.
  const { prose, restore } = pageProse(doc);
  const headings = pageHeadings(prose, ctx.headingIds, ctx.pageUrl);
  let result = transformProse(prose, ctx, headings.slugs);
  result = transformHeadingIds(result, headings.anchorIds, ctx.headingIds);

  if (authors.length) {
    result += `\n\n---\n\n*Author${authors.length > 1 ? "s" : ""}: ${authors.join(", ")}*\n`;
  }

  // Collapse runs of blank lines, also next to code. Only blank lines are
  // removed at the start, the indentation of a leading code block is kept.
  result = restore(result.replace(/\n{3,}/g, "\n\n"));
  result = result.replace(/^(?:[ \t]*\n)+/, "").trimEnd() + "\n";
  // A rule in the first line would start front matter, which the Markdown
  // processor removes up to the next rule. `***` is a rule only.
  return result.replace(/^(?:---|\+\+\+)[ \t]*\n/, "***\n");
}
