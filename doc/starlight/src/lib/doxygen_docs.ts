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
 *    (`findDoxygenDocs()`). A file refers to other files, so this step also
 *    loads what the conversion of one file needs to know about everything
 *    else: the index of all anchors.
 *
 * 2. `convertDoxygenDoc()` converts one file into a page.
 *
 * The conversion of the Markdown is in `doxygen_filter.ts`. This module does
 * not depend on Astro, `content.config.ts` puts the pages into content
 * collections.
 */

import { promises as fs } from "node:fs";
import path from "node:path";
import {
  type AnchorIndex,
  collectDoxygenAnchors,
  extractDoxygenBrief,
  extractDoxygenGroupFromDoxygen,
  extractDoxygenGroupTitleFromDoxygen,
  transformDoxygenMarkdown,
} from "./doxygen_filter.ts";

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
  /** The prefix with which the Doxygen groups of the file start */
  groupPrefix: string;
  /** Absolute URL of the page, e.g. `/boards/esp32-wroom-32/` */
  pageUrl: string;
  /** Content of the file */
  content: string;
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
        groupPrefix: source.groupPrefix,
        pageUrl: `/${source.routePrefix}/${id}/`,
        content: await fs.readFile(docPath, "utf-8"),
      });
    }
  }
  return result;
}

/* -------------------------------------------------------------------------- */
/* The site                                                                   */
/* -------------------------------------------------------------------------- */

/**
 * The documentation of boards and CPUs: all files, and what the conversion
 * of a file needs to know about the other files.
 */
export interface DoxygenSite {
  /** The doc*.md files of each source tree */
  docs: Map<DoxygenDocSource, DoxygenDocFile[]>;
  /** Global index of the Doxygen anchors of all source trees */
  anchors: AnchorIndex;
}

/**
 * Read the documentation of all source trees.
 *
 * Board documents reference sections of CPU documents (e.g. `\ref esp32_riot`)
 * and vice versa, so the anchor index has to be built from all trees before
 * any page is converted.
 */
export async function loadDoxygenSite(sources: DoxygenDocSource[]): Promise<DoxygenSite> {
  const docs = new Map<DoxygenDocSource, DoxygenDocFile[]>();

  const trees = await Promise.all(sources.map(findDoxygenDocs));
  for (const [n, source] of sources.entries()) {
    docs.set(source, trees[n]);
  }
  const files = trees.flat();

  const anchors: AnchorIndex = new Map();
  for (const doc of files) {
    collectDoxygenAnchors(doc.content, doc.groupPrefix, doc.pageUrl, anchors);
  }
  return { docs, anchors };
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
  /** Doxygen group of the page, in Doxygen's URL encoding */
  doxygenGroup: string | undefined;
  /** Content of the page as Markdown */
  markdown: string;
  /** The references of the file that are not on the site */
  unresolved: string[];
}

/** Convert a file into its page. */
export function convertDoxygenDoc(doc: DoxygenDocFile, site: DoxygenSite): DoxygenPage {
  const unresolved = new Set<string>();
  const markdown = transformDoxygenMarkdown(doc.content, {
    groupPrefix: doc.groupPrefix,
    pageUrl: doc.pageUrl,
    anchors: site.anchors,
    onUnresolved: (id) => unresolved.add(id),
  });
  return {
    title: extractDoxygenGroupTitleFromDoxygen(
      doc.content,
      doc.groupPrefix,
      doc.docName.replace(/-/g, " "),
    ),
    description: extractDoxygenBrief(doc.content, doc.groupPrefix),
    doxygenGroup: extractDoxygenGroupFromDoxygen(doc.content, doc.groupPrefix),
    markdown,
    unresolved: [...unresolved],
  };
}
