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

/**
 * Map Doxygen callout/aside commands to human-friendly labels.
 * @param command The Doxygen callout command (e.g. "warning", "note", "tip", etc.).
 * @returns A human-friendly label for the callout (e.g. "Warning", "Note", "Tip"), or null if the command is not recognized as a callout.
 */
function getCalloutLabel(command: string): string | null {
  const key = command.toLowerCase();
  if (key === "warning" || key === "attention" || key === "important") {
    return "Warning";
  }
  if (key === "note" || key === "remark") {
    return "Note";
  }
  if (key === "tip" || key === "hint") {
    return "Tip";
  }
  return null;
}

/**
 * Extract a title for a board from the Doxygen documentation content.
 * @param content The raw Doxygen markdown content of the board documentation.
 *              This is expected to contain a `@defgroup` directive with the board's title.
 * @param fallback A fallback title to use if no title can be extracted from the Doxygen content.
 *              This is typically derived from the board name.
 * @returns The extracted title, or the fallback if no title could be extracted.
 */
export function extractBoardTitleFromDoxygen(
  content: string,
  fallback: string,
): string {
  const lines = content.split("\n");
  const defgroupPattern = /@defgroup\s+boards_[A-Za-z0-9._-]+\s+(.+)$/;

  for (let i = 0; i < lines.length; i++) {
    const match = lines[i].match(defgroupPattern);
    if (!match) {
      continue;
    }
    return stripAsterisksAndQuotes(match[1]);
  }

  return fallback;
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
  const caption = stripAsterisksAndQuotes(quotedCaption?.[1] || optionless || "Image");
  return "![" + caption + "](" + imagePath + ")";
}

/**
 * Transform Doxygen markdown content to remove unsupported directives and convert some to standard markdown.
 * @param content The raw Doxygen markdown content to transform.
 * @returns The transformed markdown content with unsupported directives removed and some converted to standard markdown.
 * @warn This function does not support all Doxygen directives and may not cover all edge cases.
 *          It is intended to handle the most common directives found in RIOT board documentation,
 *          but may need to be extended in the future to support additional directives or edge cases.
 */
export function transformDoxygenMarkdown(content: string): string {
  // Split the content into lines and process each line to handle Doxygen directives.
  const sourceLines = content.split(/\r?\n/);
  const out: string[] = [];

  for (let i = 0; i < sourceLines.length; i++) {
    const rawLine = sourceLines[i];
    const line = rawLine.trim();
    const brief = transformBriefLine(line);
    const callout = transformCalloutLine(line);
    const image = transformImageLine(line);

    if (isGroupMetadata(line)) {
      // If desired, we could potentially use these to structure the content in the future,
      // but for now we just ignore them.
    } else if (brief !== null) {
      out.push(brief);
    } else if (callout !== null) {
      if (callout) {
        out.push(callout);
      }
    } else if (image !== null) {
      out.push(image);
    } else {
      out.push(stripDoxygenAnchors(replaceInlineDoxygenCommands(rawLine)));
    }
  }

  return out.join("\n");
}
