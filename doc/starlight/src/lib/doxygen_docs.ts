/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * The documentation of boards and CPUs.
 *
 * A board or CPU is documented in `doc*.md` files next to its code, e.g.
 * `boards/esp32-wroom-32/doc.md`, in the Markdown flavour of Doxygen. This
 * module makes the pages of the site from these files, in two steps:
 *
 * 1. `loadDoxygenSite()` finds the files of all source trees
 *    (`findDoxygenDocs()`). A file refers to other files and to the API
 *    documentation, so this step also loads what the conversion of one file
 *    needs to know about everything else: the index of all anchors, the
 *    index of the API documentation and the ids of the headings.
 *
 * 2. `convertDoxygenDoc()` converts one file into a page.
 *
 * The conversion of the Markdown is in `doxygen_filter.ts`, the reader of the
 * index of the API documentation is in `doxygen_tagfile.ts`. This module does
 * not depend on Astro, `content.config.ts` puts the pages into content
 * collections.
 */

import { promises as fs } from "node:fs";
import path from "node:path";
import {
  AnchorIndex,
  collectDoxygenAnchors,
  type DoxygenDoc,
  headingMarkdown,
  parseDoxygenDoc,
  transformDoxygenMarkdown,
} from "./doxygen_filter.ts";
import { type ApiIndex, doxygenGroupFile, loadDoxygenTagFile } from "./doxygen_tagfile.ts";

/* -------------------------------------------------------------------------- */
/* The files of a source tree                                                 */
/* -------------------------------------------------------------------------- */

/** A source tree with doc*.md files that are rendered as pages. */
export interface DoxygenDocSource {
  /** The folder from which to load the doc*.md files */
  dir: string;
  /** The prefix with which the Doxygen groups of these docs start */
  groupPrefix: string;
  /** The first path segment of the generated pages */
  routePrefix: string;
}

/** One doc*.md file that is rendered as a page. */
export interface DoxygenDocFile {
  /**
   * Id of the page: the name of the directory of the file, e.g. `esp32`. A
   * further file of a directory adds its name, e.g. `esp32/doc_esp32.doc`.
   */
  id: string;
  /** Path of the file, relative to the working directory */
  docPath: string;
  /** Path of the file, relative to the root of the repository */
  sourcePath: string;
  /** Directory of the file, relative to the root of the repository */
  sourceDir: string;
  /** Name of the directory of the file, e.g. `esp32-wroom-32` */
  docName: string;
  /** True for the `doc.md` of a directory, false for its further files */
  isParent: boolean;
  /** Absolute URL of the page, e.g. `/boards/esp32-wroom-32/` */
  pageUrl: string;
  /** The parsed file */
  parsed: DoxygenDoc;
}

/**
 * Path relative to the root of the repository. The source trees are given
 * relative to the working directory, `doc/starlight`.
 */
function repoPath(file: string): string {
  return file.replaceAll("../", "");
}

/**
 * Find all doc*.md files of boards (`../../boards`) or CPUs (`../../cpu`).
 */
export async function findDoxygenDocs(source: DoxygenDocSource): Promise<DoxygenDocFile[]> {
  const result: DoxygenDocFile[] = [];
  const entries = await fs.readdir(source.dir, { withFileTypes: true });

  for (const docDir of entries.filter((entry) => entry.isDirectory())) {
    const docName = docDir.name;
    const docDirPath = path.join(source.dir, docName);
    const docFiles = (await fs.readdir(docDirPath, { withFileTypes: true }))
      .filter((entry) => entry.isFile() && entry.name.endsWith("doc.md"))
      .map((entry) => entry.name)
      .sort();

    for (const docFile of docFiles) {
      const baseName = path.basename(docFile, ".md");
      const isParent = baseName === "doc";
      const id = isParent ? docName : `${docName}/${baseName}`;
      const docPath = path.join(docDirPath, docFile);
      result.push({
        id,
        docPath,
        sourcePath: repoPath(docPath),
        sourceDir: repoPath(docDirPath),
        docName,
        isParent,
        pageUrl: `/${source.routePrefix}/${id}/`,
        parsed: parseDoxygenDoc(await fs.readFile(docPath, "utf-8"), source.groupPrefix),
      });
    }
  }
  return result;
}

/* -------------------------------------------------------------------------- */
/* The API documentation                                                      */
/* -------------------------------------------------------------------------- */

/** Where the API documentation and its Doxygen tag file are. */
export interface ApiDocLocation {
  /** Base URL of the published API documentation, without a trailing `/` */
  baseUrl: string;
  /**
   * Path or URL of the tag file to use. An empty value disables the tag
   * file. If it is not set, the tag file is `localTagFile` if that file
   * exists, and else the one of the published API documentation.
   */
  tagFile?: string;
  /** Tag file of a local build of the API documentation (`make doc`) */
  localTagFile: string;
  /**
   * Path for the copy of a downloaded tag file, so that the download is only
   * repeated if the published tag file has changed
   */
  tagFileCache?: string;
}

/** Name of the tag file in the published API documentation. */
const TAGFILE_NAME = "riot.tag";

/**
 * Load the index of the API documentation from its Doxygen tag file, see
 * `ApiDocLocation` for where the tag file is taken from.
 *
 * Without a tag file the pages are still generated. References to the API
 * documentation are then rendered as code instead of links, and references
 * to groups link to the page that the name of the group implies. This is
 * also the case if the source is not a complete tag file, e.g. the error
 * page of a server.
 */
async function loadApiIndex(apiDoc: ApiDocLocation): Promise<ApiIndex | undefined> {
  const source =
    apiDoc.tagFile ??
    (await fs.access(apiDoc.localTagFile).then(
      () => apiDoc.localTagFile,
      () => `${apiDoc.baseUrl}/${TAGFILE_NAME}`,
    ));
  if (!source) {
    return undefined;
  }
  try {
    return await loadDoxygenTagFile(source, apiDoc.tagFileCache, (reason) =>
      console.warn(
        `Doxygen tag file "${source}" is not available (${reason}). ` +
          "Its copy in the cache is used, which may be outdated.",
      ),
    );
  } catch (error) {
    console.warn(
      `Doxygen tag file "${source}" is not available (${error instanceof Error ? error.message : error}). ` +
        "References to the API documentation are rendered without links.",
    );
    return undefined;
  }
}

/**
 * URL of the page of a group in the API documentation. Without a tag file
 * the file name is derived from the group name. A page without a group links
 * to the start page.
 */
function apiGroupUrl(group: string | undefined, site: DoxygenSite): string {
  if (!group) {
    return `${site.apiBaseUrl}/`;
  }
  return `${site.apiBaseUrl}/${site.api?.lookup(group)?.url ?? doxygenGroupFile(group)}`;
}

/* -------------------------------------------------------------------------- */
/* The site                                                                   */
/* -------------------------------------------------------------------------- */

/**
 * The documentation of boards and CPUs: all files, and what the conversion
 * of a file needs to know about the other files and the API documentation.
 */
export interface DoxygenSite {
  /** The doc*.md files of each source tree */
  docs: Map<DoxygenDocSource, DoxygenDocFile[]>;
  /** Global index of the Doxygen anchors of all source trees */
  anchors: AnchorIndex;
  /** Ids of the headings of each page, from the Markdown processor */
  headingIds: Map<DoxygenDocFile, string[]>;
  /** Index of the API documentation, if its tag file is available */
  api: ApiIndex | undefined;
  /** Base URL of the API documentation, without a trailing `/` */
  apiBaseUrl: string;
}

/** Render Markdown with the processor of the site, return its heading ids. */
export type HeadingIdRenderer = (markdown: string) => Promise<string[]>;

/**
 * Read the documentation of all source trees.
 *
 * Board documents reference sections of CPU documents (e.g. `\ref esp32_riot`)
 * and vice versa, so the anchor index has to be built from all trees before
 * any page is converted.
 */
export async function loadDoxygenSite(
  sources: DoxygenDocSource[],
  apiDoc: ApiDocLocation,
  renderHeadingIds: HeadingIdRenderer,
): Promise<DoxygenSite> {
  const apiPromise = loadApiIndex(apiDoc);
  const docs = new Map<DoxygenDocSource, DoxygenDocFile[]>();

  const trees = await Promise.all(sources.map(findDoxygenDocs));
  for (const [n, source] of sources.entries()) {
    docs.set(source, trees[n]);
  }
  const files = trees.flat();
  const api = await apiPromise;

  // a heading can contain a reference, so it is converted with a draft index
  const draft = new AnchorIndex();
  for (const doc of files) {
    collectDoxygenAnchors(doc.parsed, doc.pageUrl, draft);
  }
  const headingIds = new Map<DoxygenDocFile, string[]>();
  const anchors = new AnchorIndex();
  for (const doc of files) {
    const headings = headingMarkdown(doc.parsed, {
      pageUrl: doc.pageUrl,
      sourcePath: doc.sourcePath,
      anchors: draft,
      api,
      apiBaseUrl: apiDoc.baseUrl,
    });
    const ids = headings ? await renderHeadingIds(headings) : [];
    headingIds.set(doc, ids);
    collectDoxygenAnchors(doc.parsed, doc.pageUrl, anchors, ids);
  }
  for (const [id, urls] of anchors.duplicates()) {
    console.warn(
      `The anchor "${id}" is defined more than once (${urls.join(", ")}). ` +
        "A reference to it from another page links to the first definition.",
    );
  }
  return { docs, anchors, headingIds, api, apiBaseUrl: apiDoc.baseUrl };
}

/* -------------------------------------------------------------------------- */
/* Pages                                                                      */
/* -------------------------------------------------------------------------- */

/** The page that is made from a doc*.md file. */
export interface DoxygenPage {
  /** Title of the group of the file, or else the name of its directory */
  title: string;
  /** Description of the page, the `@brief` of its group */
  description: string | undefined;
  /** URL of the page of the group in the API documentation */
  apiUrl: string;
  /** Content of the page as Markdown */
  markdown: string;
  /**
   * The references of the file that could not be resolved. Empty without a
   * tag file: nothing of the API documentation can be resolved then, which
   * is reported once by `loadApiIndex()`.
   */
  unresolved: string[];
}

/** Convert a file into its page. */
export function convertDoxygenDoc(doc: DoxygenDocFile, site: DoxygenSite): DoxygenPage {
  const unresolved = new Set<string>();
  const markdown = transformDoxygenMarkdown(doc.parsed, {
    pageUrl: doc.pageUrl,
    sourcePath: doc.sourcePath,
    anchors: site.anchors,
    api: site.api,
    apiBaseUrl: site.apiBaseUrl,
    headingIds: site.headingIds.get(doc) ?? [],
    onUnresolved: (id) => unresolved.add(id),
  });
  return {
    title: doc.parsed.title || doc.docName.replace(/-/g, " "),
    description: doc.parsed.brief,
    apiUrl: apiGroupUrl(doc.parsed.groups[0], site),
    markdown,
    unresolved: site.api ? [...unresolved] : [],
  };
}
