/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Clean up text by removing leading/trailing asterisks and quotes.
 * @param value The text to clean.
 * @returns The cleaned text.
 */
function stripAsterisksAndQuotes(value: string): string {
  return value
    .replace(/^\*+\s*/, "") // Remove leading asterisks and whitespace
    .replace(/\*\/\s*$/, "") // Remove trailing */ and whitespace
    .replace(/^"|"$/g, "") // Remove leading and trailing quotes
    .trim();
}

/**
 * Replace inline Doxygen commands in a line of text with appropriate markdown formatting.
 * Currently supports:
 * - `@c` and `\c` for inline code (converted to backticks)
 * - `@ref` and `\ref` for references
 *      (converted to plain text, given that these are doxygen references
 *       we have no way of resolving them)
 * - `@p` and `\p` for parameters (converted to backticks)
 * @param line The line of text containing Doxygen commands.
 * @returns The line of text with Doxygen commands replaced by markdown formatting.
 */
function replaceInlineDoxygenCommands(line: string): string {
  return line
    .replace(/[@\\]c\s+([A-Za-z0-9_./:-]+)/g, "`$1`") // Convert @c and \c to inline code
    .replace(/[@\\]ref\s+([A-Za-z0-9_./:-]+)/g, "$1") // Convert @ref and \ref to plain text (removing the command)
    .replace(/[@\\]p\s+([A-Za-z0-9_./:-]+)/g, "`$1`"); // Convert @p and \p to inline code
}

/**
 * Remove Doxygen anchors from a line of text. (e.g. `{#TITLE}` fragments appended to headings)
 * @param line The line of text to remove Doxygen anchors from.
 * @returns The line of text with Doxygen anchors removed.
 */
function stripDoxygenAnchors(line: string): string {
  return line.replace(/\s*\{#[^}]+\}\s*/g, "").trimEnd();
}

/** Human-friendly labels for the Doxygen callout/aside commands we render. */
const CALLOUT_LABELS: Record<string, string> = {
  warning: "Warning",
  attention: "Warning",
  important: "Warning",
  note: "Note",
  remark: "Note",
  tip: "Tip",
  hint: "Tip",
};

/**
 * Map Doxygen callout/aside commands to human-friendly labels.
 * @param command The Doxygen callout command (e.g. "warning", "note", "tip", etc.).
 * @returns A human-friendly label for the callout (e.g. "Warning", "Note", "Tip"), or null if the command is not recognized as a callout.
 */
function getCalloutLabel(command: string): string | null {
  return CALLOUT_LABELS[command.toLowerCase()] ?? null;
}

/**
 * Extract a Doxygen group from the Doxygen documentation content.
 *
 * @param content The raw Doxygen markdown content of the board documentation.
 *                This is expected to contain a `@defgroup` directive with the board's title.
 * @param groupPrefix A prefix string that the group name starts with.
 * @returns The extracted group or undefined if no group could be extracted.
 */
export function extractDoxygenGroupFromDoxygen(
  text: string,
  groupPrefix: string,
): string | undefined {
  const defgroup_match = text.match(
    new RegExp(`@defgroup\\s+(${groupPrefix}_[A-Za-z0-9._-]+)\\s+`),
  );
  const doxygen_group_raw = defgroup_match?.[1];
  const doxygen_group = doxygen_group_raw?.replaceAll("_", "__");

  return doxygen_group;
}

/**
 * Extract a title of a Doxygen group from the Doxygen documentation content.
 *
 * @param content The raw Doxygen markdown content of the board documentation.
 *                This is expected to contain a `@defgroup` directive with the board's title.
 * @param groupPrefix A prefix string that the group name starts with.
 * @param fallback A fallback title to use if no title can be extracted from the Doxygen content.
 *                 This is typically derived from the board name.
 * @returns The extracted title, or the fallback if no title could be extracted.
 */
export function extractDoxygenGroupTitleFromDoxygen(
  content: string,
  groupPrefix: string,
  fallback: string,
): string {
  const match = content.match(
    new RegExp(`@defgroup\\s+${groupPrefix}_[A-Za-z0-9._-]+\\s+(.+)$`, "m"),
  );

  return match ? stripAsterisksAndQuotes(match[1]) : fallback;
}

/**
 * Check if a line of text is a Doxygen group metadata directive
 * (e.g. @defgroup, @ingroup, @addtogroup, @name).
 * @param line The line of text to check.
 * @returns True if the line is a Doxygen group metadata directive, false otherwise.
 */
function isGroupMetadata(line: string): boolean {
  return /^@(?:defgroup|ingroup|addtogroup|name)\b/.test(line.trim());
}

/**
 * Convert a Doxygen @brief line to plain markdown text.
 * @param line The trimmed input line.
 * @returns The converted text, or null if the line is not a brief directive.
 */
function transformBriefLine(line: string): string | null {
  const brief = line.match(/^[@\\]brief\s+(.+)$/);
  if (!brief) {
    return null;
  }
  return stripAsterisksAndQuotes(brief[1]);
}

/**
 * Convert Doxygen callout lines to markdown emphasis labels.
 * @param line The trimmed input line.
 * @returns The converted callout, an empty string for unknown labels, or null if not a callout line.
 */
function transformCalloutLine(line: string): string | null {
  const calloutMatch = line.match(
    /^[@\\](warning|experimental|attention|important|note|remark|tip|hint)\s+(.+)$/i,
  );
  if (!calloutMatch) {
    return null;
  }

  const label = getCalloutLabel(calloutMatch[1]);
  if (!label) {
    return "";
  }

  return `**${label}:** ${stripAsterisksAndQuotes(calloutMatch[2])}`;
}

/**
 * Convert a Doxygen @image line to markdown image syntax.
 * @param line The trimmed input line.
 * @returns The converted image markdown, or null if the line is not an image directive.
 */
function transformImageLine(line: string): string | null {
  const image = line.match(/^[@\\]image\s+\w+\s+(\S+)(?:\s+(.+))?$/);
  if (!image) {
    return null;
  }

  const imagePath = image[1];
  const imageArgs = image[2] || "";
  const quotedCaption = imageArgs.match(/"([^"]+)"/);
  const optionless = imageArgs
    .replace(/\s+[A-Za-z0-9_-]+\s*=\s*[^\s]+/g, "")
    .trim();
  const caption = stripAsterisksAndQuotes(
    quotedCaption?.[1] || optionless || "Image",
  );
  return "![" + caption + "](" + imagePath + ")";
}

/**
 * Astro site-relative path under which the images of the Doxygen source tree are
 * served.
 */
const IMAGE_BASE_PATH = "/img";

/** File extensions that are treated as images when rewriting relative paths. */
const IMAGE_EXTENSION_PATTERN = /\.(?:svg|png|jpe?g|gif|webp|avif)$/i;

/** URLs that must be kept as they are (absolute, protocol relative, data, anchors). */
const ABSOLUTE_URL_PATTERN = /^(?:[A-Za-z][A-Za-z0-9+.-]*:|\/\/|\/|#)/;

/**
 * Rewrite a single image reference to a path that the site can serve.
 *
 * Doxygen resolves image references through its IMAGE_PATH, because the docs sadly
 * only spell out the file name (e.g. `nucleo-f031k6-and-more.svg`) even though the
 * file lives in `doc/doxygen/src/pinouts/`.
 * The image will be hosted by starlight inside the `IMAGE_BASE_PATH`.
 *
 * @param target The image reference as written in the Doxygen documentation.
 * @returns The rewritten reference, or the unchanged target if it is not a relative image.
 */
function rewriteImageReference(target: string): string {
  if (
    ABSOLUTE_URL_PATTERN.test(target) ||
    !IMAGE_EXTENSION_PATTERN.test(target)
  ) {
    return target;
  }

  // Only the file name is relevant
  const fileName = target.split("/").pop();
  return `${IMAGE_BASE_PATH}/${fileName}`;
}

/**
 * Rewrite the relative image references of a line of text, both in markdown
 * image syntax (`![alt](file.svg)`) and in raw `<img>` tags, which the board
 * and CPU docs use to be able to set an image width.
 *
 * @param line The line of text to rewrite the image references in.
 * @returns The line of text with all relative image references rewritten.
 */
function rewriteImageReferences(line: string): string {
  return line
    .replace(
      /(<img\b[^>]*?\ssrc\s*=\s*(["']))(.*?)\2/gi,
      (_match, prefix: string, quote: string, target: string) =>
        `${prefix}${rewriteImageReference(target)}${quote}`,
    )
    .replace(
      /(!\[[^\]]*\]\(\s*)([^)\s]+)/g,
      (_match, prefix: string, target: string) =>
        `${prefix}${rewriteImageReference(target)}`,
    );
}

/**
 * Transform a single line of Doxygen markdown.
 * @param rawLine The untrimmed input line.
 * @returns The transformed line, or null if the line should be dropped.
 */
function transformDoxygenLine(rawLine: string): string | null {
  const line = rawLine.trim();

  // Group metadata could be used to structure the content in the future,
  // but for now we just drop it.
  if (isGroupMetadata(line)) {
    return null;
  }

  const brief = transformBriefLine(line);
  if (brief !== null) {
    return brief;
  }

  // Callout commands without a human-friendly label (e.g. @experimental)
  // yield an empty string and are dropped.
  const callout = transformCalloutLine(line);
  if (callout !== null) {
    return callout || null;
  }

  const image = transformImageLine(line);
  if (image !== null) {
    return rewriteImageReferences(image);
  }

  return rewriteImageReferences(
    stripDoxygenAnchors(replaceInlineDoxygenCommands(rawLine)),
  );
}

/**
 * Transform Doxygen markdown content to remove unsupported directives and
 * convert some to standard markdown.
 *
 * @param content The raw Doxygen markdown content to transform.
 * @returns The transformed markdown content.
 */
export function transformDoxygenMarkdown(content: string): string {
  return content
    .split(/\r?\n/)
    .map(transformDoxygenLine)
    .filter((line): line is string => line !== null)
    .join("\n");
}
