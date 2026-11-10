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
 * The conversion works in two passes:
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
 * Anything inside fenced code blocks is left untouched.
 */

/** Target of a Doxygen anchor. */
export interface AnchorTarget {
  /** Absolute site URL of the page including the fragment, e.g. `/cpus/esp32/#esp32_toc` */
  url: string;
  /** Human readable title used as link text if `\ref` has no explicit text */
  title: string;
}

/** Global map from Doxygen anchor/group names to their location on the site. */
export type AnchorIndex = Map<string, AnchorTarget>;

/** Context needed to transform one file. */
export interface TransformContext {
  /** Prefix of the Doxygen groups rendered on this site, e.g. `boards` or `cpu` */
  groupPrefix: string;
  /** Absolute URL of the page that is generated from the file, e.g. `/boards/esp32-wroom-32/` */
  pageUrl: string;
  /** Global anchor index created by `collectDoxygenAnchors()` */
  anchors: AnchorIndex;
  /** Base URL of the Doxygen API documentation used for unresolved references */
  apiBaseUrl?: string;
  /** Callback for references that cannot be resolved (for build warnings) */
  onUnresolved?: (id: string) => void;
}

const DEFAULT_API_BASE_URL = "https://api.riot-os.org";

/* -------------------------------------------------------------------------- */
/* Helpers                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * Clean up text by removing leading/trailing asterisks and quotes.
 */
function stripAsterisksAndQuotes(value: string): string {
  return value
    .replace(/^\*+\s*/, "")
    .replace(/\*\/\s*$/, "")
    .replace(/^"|"$/g, "")
    .trim();
}

/** Doxygen file name of a group page, `foo_bar` → `group__foo__bar.html` */
function doxygenGroupPage(group: string): string {
  return `group__${group.replaceAll("_", "__")}.html`;
}

/**
 * Split a document into segments of prose and fenced code so that the
 * transformations are never applied to code. Doxygen `@code` ... `@endcode`
 * blocks are converted to fenced code blocks here as well.
 */
function splitCode(content: string): { code: boolean; text: string }[] {
  const lines = content.split(/\r?\n/);
  const segments: { code: boolean; text: string }[] = [];
  let buffer: string[] = [];
  let fence: string | null = null;

  const flush = (code: boolean) => {
    if (buffer.length) {
      segments.push({ code, text: buffer.join("\n") });
      buffer = [];
    }
  };

  for (const line of lines) {
    if (fence === null) {
      const md = line.match(/^\s*(`{3,}|~{3,})/);
      const dox = line.match(/^\s*[@\\]code(?:\{\.?(\w+)\})?\s*$/);
      if (md || dox) {
        flush(false);
        fence = md ? md[1] : "@endcode";
        buffer.push(md ? line : "```" + (dox![1] ?? ""));
        continue;
      }
      buffer.push(line);
    } else {
      if (fence === "@endcode" && /^\s*[@\\]endcode\s*$/.test(line)) {
        buffer.push("```");
        fence = null;
        flush(true);
      } else if (
        fence !== "@endcode" &&
        line.trim().startsWith(fence[0].repeat(fence.length)) &&
        line.trim().replace(new RegExp(`^\\${fence[0]}+`), "") === ""
      ) {
        buffer.push(line);
        fence = null;
        flush(true);
      } else {
        buffer.push(line);
      }
    }
  }
  flush(fence !== null);
  return segments;
}

/** Apply `fn` only to the prose parts of `content`. */
function mapProse(content: string, fn: (text: string) => string): string {
  return splitCode(content)
    .map((s) => (s.code ? s.text : fn(s.text)))
    .join("\n");
}

/* -------------------------------------------------------------------------- */
/* Group handling                                                             */
/* -------------------------------------------------------------------------- */

interface GroupBlock {
  name: string;
  title: string;
  brief?: string;
  authors: string[];
  body: string;
}

/**
 * Split a file at its `@defgroup` lines. Files like `cpu/esp32/doc.md` define
 * more than one group (e.g. `stdio_usb_serial_jtag` *and* `cpu_esp32`), and
 * only the groups with the site's prefix belong on the generated page.
 */
function splitGroups(content: string): GroupBlock[] {
  const withoutComments = content.replace(/<!--[\s\S]*?-->/g, "");
  const parts = withoutComments.split(/^(?=[ \t]*[@\\]defgroup\s)/m);
  const blocks: GroupBlock[] = [];

  for (const part of parts) {
    const def = part.match(/^[ \t]*[@\\]defgroup\s+(\S+)\s+(.*)$/m);
    if (!def) {
      continue; // text before the first @defgroup (licence header etc.)
    }
    const brief = part.match(/^[ \t]*[@\\]brief\s+(.+)$/m)?.[1];
    const authors = [...part.matchAll(/^[ \t]*[@\\]author\s+(.+)$/gm)].map(
      (m) => m[1].replace(/<[^>]*@[^>]*>/, "").trim(), // drop e-mail address
    );
    const body = part
      .split("\n")
      .filter(
        (line) =>
          !/^\s*[@\\](defgroup|ingroup|addtogroup|brief|author|name)\b/.test(
            line,
          ),
      )
      .join("\n");
    blocks.push({
      name: def[1],
      title: stripAsterisksAndQuotes(def[2]),
      brief: brief ? stripAsterisksAndQuotes(brief) : undefined,
      authors,
      body,
    });
  }
  return blocks;
}

/** Return the group blocks that belong to the site (matching `prefix_`). */
function siteGroups(content: string, prefix: string): GroupBlock[] {
  const blocks = splitGroups(content);
  const own = blocks.filter((b) => b.name.startsWith(`${prefix}_`));
  return own.length ? own : blocks;
}

/**
 * Extract the Doxygen group (in Doxygen's URL encoding) of the page.
 */
export function extractDoxygenGroupFromDoxygen(
  text: string,
  groupPrefix: string,
): string | undefined {
  return siteGroups(text, groupPrefix)[0]?.name.replaceAll("_", "__");
}

/**
 * Extract the title of the page from its `@defgroup`.
 */
export function extractDoxygenGroupTitleFromDoxygen(
  content: string,
  groupPrefix: string,
  fallback: string,
): string {
  return siteGroups(content, groupPrefix)[0]?.title || fallback;
}

/**
 * Extract the description of the page from the `@brief` of its own group
 * (not from the first `@brief` in the file, which may belong to another group).
 */
export function extractDoxygenBrief(
  content: string,
  groupPrefix: string,
): string | undefined {
  return siteGroups(content, groupPrefix)[0]?.brief;
}

/* -------------------------------------------------------------------------- */
/* Pass 1: anchor collection                                                  */
/* -------------------------------------------------------------------------- */

const SECTION_RE = /^[ \t]*[@\\](section|subsection|subsubsection|paragraph)\s+(\S+)\s+(.+)$/gm;
const HEADING_ID_RE = /^#{1,6}[ \t]+(.*?)[ \t]*\{#([A-Za-z0-9_.-]+)\}[ \t]*$/gm;
const ANCHOR_RE = /^[ \t]*[@\\]anchor\s+(\S+)\s*$/gm;

/**
 * Record all anchors defined in a file. Must be called for every file before
 * the first call of `transformDoxygenMarkdown()`.
 *
 * Note: anchors are collected from *all* groups of a file, also from groups
 * that are not rendered on the site. References to such anchors are then
 * linked to the API documentation (see `resolveRef()`).
 */
export function collectDoxygenAnchors(
  content: string,
  groupPrefix: string,
  pageUrl: string,
  index: AnchorIndex,
): void {
  const own = new Set(siteGroups(content, groupPrefix).map((b) => b.name));

  for (const block of splitGroups(content)) {
    if (!own.has(block.name)) {
      continue;
    }
    index.set(block.name, { url: pageUrl, title: block.title });

    const prose = splitCode(block.body)
      .filter((s) => !s.code)
      .map((s) => s.text)
      .join("\n");

    let first = true;
    for (const m of prose.matchAll(SECTION_RE)) {
      // The first \section of a document is its title section, which is
      // dropped from the page, so it points to the top of the page.
      const isTitle = first && m[1] === "section" && isLeadingSection(prose, m.index!);
      index.set(m[2], {
        url: isTitle ? pageUrl : `${pageUrl}#${m[2]}`,
        title: stripAsterisksAndQuotes(m[3]),
      });
      first = false;
    }
    for (const m of prose.matchAll(HEADING_ID_RE)) {
      index.set(m[2], { url: `${pageUrl}#${m[2]}`, title: m[1].trim() });
    }
    for (const m of prose.matchAll(ANCHOR_RE)) {
      index.set(m[1], { url: `${pageUrl}#${m[1]}`, title: m[1] });
    }
  }
}

/** True if only whitespace precedes position `pos` in `text`. */
function isLeadingSection(text: string, pos: number): boolean {
  return text.slice(0, pos).trim() === "";
}

/* -------------------------------------------------------------------------- */
/* Pass 2: transformation                                                     */
/* -------------------------------------------------------------------------- */

/** Make a URL relative to the current page if it points to the same page. */
function localize(url: string, ctx: TransformContext): string {
  return url.startsWith(`${ctx.pageUrl}#`) ? url.slice(ctx.pageUrl.length) : url;
}

/**
 * Resolve a `\ref` target. Order:
 * 1. anchors and groups rendered on this site (global anchor index)
 * 2. anything else is assumed to live in the API documentation
 *    (groups → group page, everything else → API search)
 */
function resolveRef(id: string, ctx: TransformContext): AnchorTarget {
  const target = ctx.anchors.get(id);
  if (target) {
    return { url: localize(target.url, ctx), title: target.title };
  }
  ctx.onUnresolved?.(id);
  const api = ctx.apiBaseUrl ?? DEFAULT_API_BASE_URL;
  // Without a Doxygen tag file the best guess for unknown ids is a group page
  // for group-like names. With GENERATE_TAGFILE this can be resolved exactly.
  if (/^[a-z][a-z0-9_]*$/.test(id) && !/\(/.test(id)) {
    return { url: `${api}/${doxygenGroupPage(id)}`, title: id };
  }
  return { url: `${api}/search.html?query=${encodeURIComponent(id)}`, title: id };
}

/**
 * `\ref id "text"`, `@ref id`, also with the text on the next line.
 */
function transformRefs(text: string, ctx: TransformContext): string {
  return text.replace(
    /[@\\]ref\s+([A-Za-z0-9_.:#~-]*[A-Za-z0-9_](?:\(\))?)(?:[ \t]*\n?[ \t]*"([^"\n]+)")?/g,
    (_m, id: string, label?: string) => {
      const target = resolveRef(id, ctx);
      const text = label ?? target.title;
      return `[${text}](${target.url})`;
    },
  );
}

/**
 * Markdown links to `#anchor` that is defined on another page (e.g. links
 * from `doc_esp32.doc.md` to sections of `doc.md`) are rewritten to absolute
 * links.
 */
function transformFragmentLinks(text: string, ctx: TransformContext): string {
  return text.replace(/\]\(#([A-Za-z0-9_.-]+)\)/g, (m, id: string) => {
    const target = ctx.anchors.get(id);
    return target ? `](${localize(target.url, ctx)})` : m;
  });
}

/**
 * Doxygen sections. The leading `\section` repeats the page title and is
 * dropped. Other sections become headings with an explicit anchor.
 */
function transformSections(text: string): string {
  let first = true;
  return text.replace(SECTION_RE, (_m, kind: string, id: string, title: string, offset: number) => {
    const leading = first && kind === "section" && isLeadingSection(text, offset);
    first = false;
    if (leading) {
      return "";
    }
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
  const prose = splitCode(text).filter((s) => !s.code).map((s) => s.text).join("\n");
  const levels = [...prose.matchAll(/^(#{1,6})[ \t]/gm)].map((m) => m[1].length);
  if (!levels.length) {
    return text;
  }
  const shift = Math.max(0, 2 - Math.min(...levels));
  if (!shift) {
    return text;
  }
  return mapProse(text, (t) =>
    t.replace(/^(#{1,6})([ \t])/gm, (_m, hashes: string, ws: string) =>
      "#".repeat(Math.min(6, hashes.length + shift)) + ws,
    ),
  );
}

/**
 * Explicit heading ids `{#id}`. Doxygen uses them as link targets, so they
 * have to survive. They are rendered as an empty anchor inside the heading,
 * which works with any Markdown processor and keeps the slug based ids that
 * Starlight uses for its TOC. If a processor plugin for heading attributes
 * (e.g. `satteri-custom-header-id`) is used instead, this function can simply
 * return the text unchanged.
 */
function transformHeadingIds(text: string): string {
  return text.replace(
    /^(#{1,6}[ \t]+.*?)[ \t]*\{#([A-Za-z0-9_.-]+)\}[ \t]*$/gm,
    (_m, heading: string, id: string) => `${heading} <a id="${id}"></a>`,
  );
}

/** `\anchor id` → empty HTML anchor */
function transformAnchors(text: string): string {
  return text.replace(ANCHOR_RE, (_m, id: string) => `<a id="${id}"></a>`);
}

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

/**
 * `@note`, `@warning`, … paragraphs. In Doxygen such a paragraph ends at the
 * next blank line (or the next paragraph command). Both forms are used:
 *
 *     @note Text that continues
 *           on the next lines.
 *
 *     @note
 *     - list item
 *     - list item
 *
 * They are converted to Starlight asides (`:::note` … `:::`).
 */
function transformCallouts(text: string): string {
  const lines = text.split("\n");
  const out: string[] = [];
  const startRe = new RegExp(
    `^\\s*[@\\\\](${Object.keys(CALLOUTS).join("|")})\\b[ \\t]*(.*)$`,
    "i",
  );

  for (let i = 0; i < lines.length; i++) {
    const start = lines[i].match(startRe);
    if (!start) {
      out.push(lines[i]);
      continue;
    }
    const callout = CALLOUTS[start[1].toLowerCase()];
    const body: string[] = [];
    if (start[2].trim()) {
      body.push(start[2].trim());
    }
    if (start[1].toLowerCase() === "experimental" && !body.length) {
      body.push("This feature is experimental!");
    }
    // collect the paragraph: everything up to the next blank line or the next
    // paragraph command
    while (
      i + 1 < lines.length &&
      lines[i + 1].trim() !== "" &&
      !startRe.test(lines[i + 1])
    ) {
      body.push(lines[++i]);
    }
    // continuation lines of `@note text` are indented to align with the text;
    // remove the common indentation so that they don't become code blocks
    const indent = Math.min(
      ...body.slice(1).filter((l) => l.trim()).map((l) => l.match(/^\s*/)![0].length),
    );
    const normalized = body.map((l, n) =>
      n === 0 || !Number.isFinite(indent) ? l : l.slice(indent),
    );
    out.push(
      "",
      `:::${callout.type}${callout.title ? `[${callout.title}]` : ""}`,
      ...normalized,
      ":::",
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
 */
function transformHtmlBlocks(text: string): string {
  return text.replace(/<\/?center>/gi, "");
}

/**
 * Doxygen accepts table rows that are indented by four or more spaces (e.g.
 * right aligned numbers in the first column), some Markdown parsers end the
 * table there. Leading whitespace of table rows is therefore removed.
 */
function transformTables(text: string): string {
  const lines = text.split("\n");
  let inTable = false;
  for (let i = 0; i < lines.length; i++) {
    if (/^\s*\|?\s*:?-{3,}:?\s*(\|\s*:?-{3,}:?\s*)+\|?\s*$/.test(lines[i])) {
      inTable = true;
      lines[i - 1] = lines[i - 1]?.trimStart();
    } else if (inTable && lines[i].includes("|") && lines[i].trim() !== "") {
      lines[i] = lines[i].trimStart();
      continue;
    } else {
      inTable = false;
    }
    if (inTable) lines[i] = lines[i].trimStart();
  }
  return lines.join("\n");
}

/** Inline commands: `\c`, `\p`, `\b`, `\e`, `\em`, `\a` */
function transformInlineCommands(text: string): string {
  return text
    .replace(/[@\\][cp]\s+([A-Za-z0-9_./:()-]+)/g, "`$1`")
    .replace(/[@\\]b\s+([^\s*]+)/g, "**$1**")
    .replace(/[@\\](?:e|em|a)\s+([A-Za-z0-9_./:-]+)/g, "*$1*")
    // `\b *` is used as a literal "*" marker in footnotes of some tables
    .replace(/[@\\]b\s+\*/g, "**\\***");
}

/**
 * Doxygen auto-links symbols prefixed by `#` (e.g. `#PWM_CH_NUMOF_MAX`).
 * Without a tag file they are rendered as code.
 */
function transformSymbolLinks(text: string): string {
  return text.replace(/(^|[\s(])#([A-Z][A-Z0-9_]{2,})\b/g, "$1`$2`");
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
    .replace(/^[ \t]*[@\\]image\s+\w+\s+(\S+)(?:\s+(.+))?$/gm, (_m, file: string, args = "") => {
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

/**
 * Transform the Doxygen Markdown of a `doc*.md` file into Markdown for the
 * generated page.
 */
export function transformDoxygenMarkdown(
  content: string,
  ctx: TransformContext,
): string {
  const blocks = siteGroups(content, ctx.groupPrefix);
  const authors = [...new Set(blocks.flatMap((b) => b.authors))];

  // further groups of the same file (e.g. `cpu_efm32_*` in `cpu/efm32/doc.md`)
  // are appended as sections of the page
  const body = blocks
    .map((b, n) =>
      n === 0 ? b.body : `\n# ${b.title} {#${b.name}}\n\n${b.brief ?? ""}\n${b.body}`,
    )
    .join("\n");

  let result = mapProse(body, (text) => {
    text = transformSections(text);
    text = transformHtmlBlocks(text);
    text = transformTables(text);
    text = transformCallouts(text);
    text = transformRefs(text, ctx);
    text = transformFragmentLinks(text, ctx);
    text = transformAnchors(text);
    text = transformInlineCommands(text);
    text = transformSymbolLinks(text);
    text = transformImages(text);
    return text;
  });

  // heading operations need the complete document (levels are relative)
  result = mapProse(shiftHeadings(result), transformHeadingIds);

  if (authors.length) {
    result += `\n\n---\n\n*Author${authors.length > 1 ? "s" : ""}: ${authors.join(", ")}*\n`;
  }

  return result.replace(/\n{3,}/g, "\n\n").trim() + "\n";
}
