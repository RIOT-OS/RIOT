/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Reader for Doxygen tag files.
 *
 * A tag file is the index that Doxygen writes with `GENERATE_TAGFILE`. It
 * lists every group, page, section anchor, function, macro, type and variable
 * of the API documentation together with the HTML file and the anchor it is
 * documented at. It is used to resolve `\ref` targets that are not part of
 * this site to exact URLs of the API documentation.
 */

import { promises as fs } from "node:fs";
import path from "node:path";

/** Location of a documented entity in the API documentation. */
export interface ApiTarget {
  /** URL relative to the base URL of the API documentation, e.g. `group__foo.html#ga123` */
  url: string;
  /** Title of groups, pages and sections. Symbols don't have a title. */
  title?: string;
  /** True for code symbols (functions, macros, types, …), which are rendered as code */
  symbol: boolean;
  /** True for macros, which are also linked with arguments (`SPI_DEV(0)`) */
  macro?: boolean;
}

/** One of several definitions of a symbol name. */
interface ApiCandidate {
  /** Repository relative path of the defining file, empty if unknown */
  file: string;
  url: string;
  /** True for a macro (`#define`) */
  macro: boolean;
}

/** Link target of a symbol definition. */
function symbolTarget({ url, macro }: ApiCandidate): ApiTarget {
  return { url, symbol: true, macro };
}

/**
 * The number of leading path segments two files share. A missing path shares
 * nothing with anything.
 */
function pathAffinity(file: string, referrerFile: string): number {
  if (!file || !referrerFile) {
    return 0;
  }
  const a = file.split("/");
  const b = referrerFile.split("/");
  let shared = 0;
  while (shared < a.length && shared < b.length && a[shared] === b[shared]) {
    shared++;
  }
  return shared;
}

/**
 * Among the definitions of a contested name, the one of the referrer's own
 * board or CPU: the only definition inside the directory of the referring
 * file (e.g. `boards/foo/include/board.h` for `boards/foo/doc.md`).
 *
 * Sharing a parent directory is not nearness. `cpu/`, `boards/` and
 * `boards/common/` are categories: `cpu/qn908x/doc.md` is no closer to
 * `cpu/gd32v/include/periph_cpu.h` than to the header of any other CPU, and
 * `boards/common/nucleo64/doc.md` is no closer to the `board.h` of
 * `boards/common/seeedstudio-xiao-nrf52840` than to that of any other board.
 */
function closestCandidate(
  candidates: ApiCandidate[],
  referrerFile: string,
): ApiCandidate | undefined {
  const referrerDepth = referrerFile.split("/").length - 1;
  if (referrerDepth < 1) {
    return undefined;
  }
  const own = candidates.filter(
    (candidate) => pathAffinity(candidate.file, referrerFile) >= referrerDepth,
  );
  return own.length === 1 ? own[0] : undefined;
}

/**
 * Index of the API documentation: Doxygen names (group, page, anchor, symbol)
 * and their location.
 *
 * Symbol names are not unique, see `parseDoxygenTagFile()` for how they are
 * resolved.
 */
export class ApiIndex {
  /** Unique names, and the canonical definition of contested names */
  private readonly targets = new Map<string, ApiTarget>();
  /** All definitions of names that are defined more than once */
  private readonly contested = new Map<string, ApiCandidate[]>();

  /** Number of names that resolve without a referring file. */
  get size(): number {
    return this.targets.size;
  }

  has(name: string): boolean {
    return this.targets.has(name) || this.contested.has(name);
  }

  set(name: string, target: ApiTarget): void {
    this.targets.set(name, target);
  }

  /** Register the definitions of a symbol name. */
  setSymbol(name: string, candidates: ApiCandidate[]): void {
    if (candidates.length === 1) {
      this.targets.set(name, symbolTarget(candidates[0]));
      return;
    }
    this.contested.set(name, candidates);
    const canonical = pickCanonical(candidates);
    if (canonical) {
      this.targets.set(name, symbolTarget(canonical));
    }
  }

  /**
   * Resolve a name.
   *
   * @param name         Name of a group, page, anchor or symbol
   * @param referrerFile Repository relative path of the file that contains
   *                     the reference, used to resolve contested names
   */
  lookup(name: string, referrerFile?: string): ApiTarget | undefined {
    const candidates = this.contested.get(name);
    if (candidates && referrerFile) {
      const closest = closestCandidate(candidates, referrerFile);
      if (closest) {
        return symbolTarget(closest);
      }
    }
    return this.targets.get(name);
  }
}

/**
 * Group pages of hardware specific code. CPUs and boards override symbols of
 * the generic API (e.g. `GPIO_PIN`), so their definitions are not canonical.
 */
const HARDWARE_GROUP_PAGE = /^group__(?:cpu|boards)(?:__|\.html)/;

/**
 * The canonical definition of a contested name: the one that is documented on
 * the page of a generic group (e.g. `drivers_periph_gpio`), if there is
 * exactly one. Definitions in file documentation (`periph__cpu_8h.html`) and
 * on CPU and board group pages are hardware specific overrides.
 */
function pickCanonical(candidates: ApiCandidate[]): ApiCandidate | undefined {
  const generic = candidates.filter(
    (c) => c.url.startsWith("group__") && !HARDWARE_GROUP_PAGE.test(c.url),
  );
  return generic.length === 1 ? generic[0] : undefined;
}

function decodeXml(text: string): string {
  return text
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&#(\d+);/g, (_m, code: string) => String.fromCodePoint(Number(code)))
    .replace(/&#x([0-9a-f]+);/gi, (_m, code: string) =>
      String.fromCodePoint(parseInt(code, 16)),
    )
    .replace(/&amp;/g, "&");
}

/**
 * File name of the page of a Doxygen group, `foo_bar` → `group__foo__bar.html`.
 * The tag file knows the exact name. This is the name Doxygen uses with
 * `CASE_SENSE_NAMES = YES` for a group name of letters, digits, `_` and `-`,
 * for use without a tag file.
 */
export function doxygenGroupFile(group: string): string {
  return `group__${group.replaceAll("_", "__")}.html`;
}

/** Old Doxygen versions write compound file names without the extension. */
function htmlFile(name: string): string {
  return /\.[A-Za-z0-9]+$/.test(name) ? name : `${name}.html`;
}

/** Compounds that are link targets on their own. */
const COMPOUND_KINDS = new Set(["group", "page", "struct", "union", "class", "namespace"]);
/** Compounds whose members are qualified with the compound name (`foo_t::bar`). */
const SCOPED_KINDS = new Set(["struct", "union", "class", "namespace"]);

/**
 * Parse a Doxygen tag file.
 *
 * Doxygen writes one XML element per line, so the file is scanned line by
 * line instead of building a DOM. This keeps the parser free of dependencies
 * and fast enough for the tag file of the complete RIOT API documentation.
 *
 * Names of groups, pages and anchors are unique in Doxygen. Symbol names are
 * not: `LED0_PIN` is defined by every board, and `GPIO_PIN` is defined by the
 * generic GPIO API and overridden by many CPUs. A symbol that is documented at
 * more than one location resolves to the definition inside the directory of
 * the referring file, see `closestCandidate()`, or else to the definition of
 * the generic API, see `pickCanonical()`. If there is neither, the symbol is
 * not resolved, because a wrong link is worse than no link.
 */
export function parseDoxygenTagFile(xml: string): ApiIndex {
  const index = new ApiIndex();
  /** symbol name → URL → definition */
  const symbols = new Map<string, Map<string, ApiCandidate>>();

  // A member is listed in the compound of its file and, if it belongs to a
  // group, again in the compound of the group. Only the file compound knows
  // the defining file.
  const addSymbol = (name: string, candidate: ApiCandidate) => {
    let urls = symbols.get(name);
    if (!urls) {
      symbols.set(name, (urls = new Map()));
    }
    if (candidate.file || !urls.has(candidate.url)) {
      urls.set(candidate.url, candidate);
    }
  };

  let compoundKind = "";
  let compoundName = "";
  let compoundPath = "";
  let compoundTitle: string | undefined;
  let inMember = false;
  let memberKind = "";
  let memberName = "";
  let memberFile = "";
  let memberAnchor = "";

  // An anchor (`\anchor`, `\section`) in the documentation of a compound or
  // of one of its members
  const addDocAnchor = (attributes: string, name: string) => {
    const file = attributes.match(/file="([^"]*)"/)?.[1];
    const title = attributes.match(/title="([^"]*)"/)?.[1];
    if (file && name && !index.has(name)) {
      index.set(name, {
        url: `${htmlFile(decodeXml(file))}#${name}`,
        title: title ? decodeXml(title) : undefined,
        symbol: false,
      });
    }
  };

  for (const rawLine of xml.split("\n")) {
    const line = rawLine.trim();

    if (line.startsWith("<compound ")) {
      compoundKind = line.match(/kind="([^"]+)"/)?.[1] ?? "";
      compoundName = "";
      compoundPath = "";
      compoundTitle = undefined;
      continue;
    }
    if (line.startsWith("<member ")) {
      inMember = true;
      memberKind = line.match(/kind="([^"]+)"/)?.[1] ?? "";
      memberName = memberFile = memberAnchor = "";
      continue;
    }
    if (line === "</member>") {
      inMember = false;
      if (memberName && memberFile) {
        const url = memberAnchor ? `${htmlFile(memberFile)}#${memberAnchor}` : htmlFile(memberFile);
        if (SCOPED_KINDS.has(compoundKind)) {
          // As in Doxygen, a bare name never means the field of a struct.
          // `gpio_init` is the function, also if a struct has a field of
          // that name. A field is referenced as `type::field`.
          if (compoundName) {
            addSymbol(`${compoundName}::${memberName}`, { file: "", url, macro: false });
          }
        } else {
          const file = compoundKind === "file" ? compoundPath + compoundName : "";
          addSymbol(memberName, { file, url, macro: memberKind === "define" });
        }
      }
      continue;
    }

    const element = line.match(/^<(\w+)([^>]*)>(.*)<\/\1>$/);
    if (!element) {
      continue;
    }
    const [, tag, attributes, rawValue] = element;
    const value = decodeXml(rawValue);

    if (inMember) {
      if (tag === "name") memberName = value;
      else if (tag === "anchorfile") memberFile = value;
      else if (tag === "anchor") memberAnchor = value;
      else if (tag === "docanchor") addDocAnchor(attributes, value);
      continue;
    }

    switch (tag) {
      case "name":
        compoundName = value;
        break;
      case "path":
        compoundPath = value;
        break;
      case "title":
        compoundTitle = value;
        break;
      case "filename":
        if (COMPOUND_KINDS.has(compoundKind) && compoundName) {
          const isDoc = compoundKind === "group" || compoundKind === "page";
          index.set(compoundName, {
            url: htmlFile(value),
            title: isDoc ? compoundTitle : undefined,
            symbol: !isDoc,
          });
        }
        break;
      case "docanchor":
        addDocAnchor(attributes, value);
        break;
    }
  }

  for (const [name, urls] of symbols) {
    if (!index.has(name)) {
      index.setSymbol(name, [...urls.values()]);
    }
  }
  return index;
}

/**
 * Parse a tag file and make sure that it is one. The error page of a server
 * and a file that is cut off are not: an index without names, or with a part
 * of the names only, turns the references into text.
 */
function parseCompleteTagFile(xml: string): ApiIndex {
  if (!xml.trimEnd().endsWith("</tagfile>")) {
    throw new Error("not a complete Doxygen tag file");
  }
  const index = parseDoxygenTagFile(xml);
  if (!index.size) {
    throw new Error("the Doxygen tag file is empty");
  }
  return index;
}

/** The longest time a server may take to send the next part of its response. */
const DOWNLOAD_TIMEOUT_MS = 30_000;

/** What is known about the copy of a download in the cache. */
interface CacheInfo {
  /** URL the file was downloaded from */
  url: string;
  etag?: string;
  lastModified?: string;
}

/** A tag file in the cache, with the validators of its download. */
interface CachedTagFile extends CacheInfo {
  index: ApiIndex;
}

/** The file that describes the cached download. */
function cacheInfoFile(cacheFile: string): string {
  return `${cacheFile}.json`;
}

/**
 * Read the cached copy of the tag file of `url`. There is none if the cache
 * is empty, belongs to another URL or is damaged.
 */
async function readCachedTagFile(cacheFile: string, url: string): Promise<CachedTagFile | undefined> {
  try {
    const info: CacheInfo = JSON.parse(await fs.readFile(cacheInfoFile(cacheFile), "utf-8"));
    if (info.url !== url) {
      return undefined;
    }
    return { ...info, index: parseCompleteTagFile(await fs.readFile(cacheFile, "utf-8")) };
  } catch {
    return undefined;
  }
}

/**
 * Write a download to the cache. The tag file is renamed into place, so that
 * a build that is interrupted doesn't leave a part of it.
 */
async function writeCachedTagFile(cacheFile: string, xml: string, info: CacheInfo): Promise<void> {
  await fs.mkdir(path.dirname(cacheFile), { recursive: true });
  // without the description the cache is not used
  await fs.rm(cacheInfoFile(cacheFile), { force: true });
  await fs.writeFile(`${cacheFile}.tmp`, xml);
  await fs.rename(`${cacheFile}.tmp`, cacheFile);
  await fs.writeFile(cacheInfoFile(cacheFile), JSON.stringify(info));
}

/**
 * Download a tag file.
 *
 * The time limit is not for the whole download but for each part of it: the
 * server has to answer in time, and then to keep sending. A slow connection
 * is not an error.
 *
 * @param cached The copy in the cache. The server only sends the file if it
 *               is newer than this copy.
 * @returns The content of the file with its validators, or `undefined` if
 *          the copy in the cache is up to date.
 */
async function downloadTagFile(
  url: string,
  cached?: CacheInfo,
): Promise<{ xml: string; info: CacheInfo } | undefined> {
  const controller = new AbortController();
  let timer: ReturnType<typeof setTimeout> | undefined;
  const restartTimer = () => {
    clearTimeout(timer);
    timer = setTimeout(
      () => controller.abort(new Error(`no data for ${DOWNLOAD_TIMEOUT_MS / 1000} s`)),
      DOWNLOAD_TIMEOUT_MS,
    );
  };

  const headers: Record<string, string> = {};
  if (cached?.etag) {
    headers["If-None-Match"] = cached.etag;
  } else if (cached?.lastModified) {
    headers["If-Modified-Since"] = cached.lastModified;
  }

  restartTimer();
  try {
    const response = await fetch(url, { headers, signal: controller.signal });
    if (response.status === 304 && cached) {
      await response.body?.cancel();
      return undefined;
    }
    if (!response.ok) {
      throw new Error(`HTTP ${response.status} ${response.statusText}`.trim());
    }
    const decoder = new TextDecoder();
    let xml = "";
    const reader = response.body?.getReader();
    for (let part = await reader?.read(); part && !part.done; part = await reader!.read()) {
      restartTimer();
      xml += decoder.decode(part.value, { stream: true });
    }
    xml += decoder.decode();
    return {
      xml,
      info: {
        url,
        etag: response.headers.get("etag") ?? undefined,
        lastModified: response.headers.get("last-modified") ?? undefined,
      },
    };
  } finally {
    clearTimeout(timer);
  }
}

/**
 * Load a tag file from a local path or from an `http(s)` URL.
 *
 * A download is kept in `cacheFile`. The next call only downloads the file
 * again if the server has a newer one. If the server can't be reached or
 * doesn't send a tag file, the copy in the cache is used.
 *
 * @param source    Path or URL of the tag file
 * @param cacheFile Path of the copy of a download, no cache if omitted
 * @param onStale   Called with the reason if the copy in the cache is used
 *                  because the download failed
 * @returns The parsed index. Throws if there is no complete tag file.
 */
export async function loadDoxygenTagFile(
  source: string,
  cacheFile?: string,
  onStale?: (reason: string) => void,
): Promise<ApiIndex> {
  if (!/^https?:\/\//.test(source)) {
    return parseCompleteTagFile(await fs.readFile(source, "utf-8"));
  }
  const cached = cacheFile ? await readCachedTagFile(cacheFile, source) : undefined;
  try {
    const download = await downloadTagFile(source, cached);
    if (!download) {
      return cached!.index;
    }
    const index = parseCompleteTagFile(download.xml);
    if (cacheFile) {
      // a cache that can't be written only costs the next download
      await writeCachedTagFile(cacheFile, download.xml, download.info).catch(() => {});
    }
    return index;
  } catch (error) {
    if (!cached) {
      throw error;
    }
    onStale?.(error instanceof Error ? error.message : String(error));
    return cached.index;
  }
}
