/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

import { defineCollection } from "astro:content";
import { type Loader, type LoaderContext } from "astro/loaders";
import { docsSchema } from "@astrojs/starlight/schema";
import { promises as fs } from "node:fs";
import { glob } from "astro/loaders";
import { z } from "astro/zod";
import path from "node:path";
import { pathToFileURL } from "node:url";
import {
  convertDoxygenDoc,
  type DoxygenDocSource,
  type DoxygenSite,
  loadDoxygenSite,
} from "./lib/doxygen_docs";
import { REPOSITORY_URL, SOURCE_TREE_URL } from "../config";

/* -------------------------------------------------------------------------- */
/* Sources of the board and CPU documentation                                 */
/* -------------------------------------------------------------------------- */

const BOARDS: DoxygenDocSource = {
  dir: "../../boards",
  groupPrefix: "boards",
  routePrefix: "boards",
};
const CPUS: DoxygenDocSource = {
  dir: "../../cpu",
  groupPrefix: "cpu",
  routePrefix: "cpus",
};

/* -------------------------------------------------------------------------- */
/* Collections                                                                */
/* -------------------------------------------------------------------------- */

const DoxygenDocSchema = z.object({
  title: z.string(),
  description: z.string().nullish(),
  doxygenGroup: z.string(),
  githubFolder: z.string(),
  editUrl: z.string(),
  includeInOverview: z.boolean(),
  parentId: z.string().nullable(),
});

export const collections = {
  docs: defineCollection({
    loader: glob({ pattern: "**/*.(md|mdx)", base: "../guides" }),
    schema: docsSchema(),
  }),
  boards: defineCollection({
    loader: doxygenDocLoader(BOARDS),
    schema: DoxygenDocSchema
  }),
  cpus: defineCollection({
    loader: doxygenDocLoader(CPUS),
    schema: DoxygenDocSchema
  }),
  changelog: defineCollection({
    loader: changelogLoader(),
    schema: z.object({
      title: z.string(),
      release: z.string(),
      codeName: z.string(),
      slug: z.string(),
      date: z.date(),
      markdown: z.string(),
    }),
  }),
};

/* -------------------------------------------------------------------------- */
/* Loaders                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * The board and the CPU documentation refer to each other, so both
 * collection loaders work on the same data, see `loadDoxygenSite()`.
 *
 * The files are read once per build and shared between both loaders. A
 * loader that runs again (e.g. in `astro dev`) gets freshly read files
 * instead of the data of its previous run.
 */
let doxygenSite: { promise: Promise<DoxygenSite>; consumers: Set<DoxygenDocSource> } | undefined;

/**
 * @param consumer The source tree of the loader that asks for the data
 * @param context  Context of this loader
 */
function getDoxygenSite(consumer: DoxygenDocSource, context: LoaderContext): Promise<DoxygenSite> {
  if (!doxygenSite || doxygenSite.consumers.has(consumer)) {
    const renderHeadingIds = async (markdown: string): Promise<string[]> => {
      const { metadata } = await context.renderMarkdown(markdown);
      return metadata?.headings?.map((heading) => heading.slug) ?? [];
    };
    doxygenSite = {
      promise: loadDoxygenSite([BOARDS, CPUS], renderHeadingIds),
      consumers: new Set(),
    };
  }
  doxygenSite.consumers.add(consumer);
  return doxygenSite.promise;
}

/**
 * Load documentation from doc*.md files of a source tree.
 * Each subdirectory may contain one or more files matching the pattern
 * `doc*.md` (e.g. `doc.md`, `doc_esp32.doc.md`).
 *
 * The files are found and converted by `lib/doxygen_docs.ts`. This loader
 * puts the pages into the collection.
 */
function doxygenDocLoader(source: DoxygenDocSource): Loader {
  return {
    name: `${source.groupPrefix}-loader`,
    load: async (context): Promise<void> => {
      try {
        const site = await getDoxygenSite(source, context);

        // the pages of files that are gone must not stay in the collection
        context.store.clear();
        for (const doc of site.docs.get(source) ?? []) {
          const page = convertDoxygenDoc(doc, site);
          if (page.unresolved.length) {
            context.logger.debug(
              `${doc.docPath}: unresolved references: ${page.unresolved.join(", ")}`,
            );
          }

          context.store.set({
            id: doc.id,
            filePath: doc.sourcePath,
            data: {
              title: page.title,
              description: page.description,
              doxygenGroup: page.doxygenGroup,
              githubFolder: doc.sourceDir,
              editUrl: `${SOURCE_TREE_URL}/${doc.sourcePath}`,
              // Only the parent appears in the overview; children are hidden
              // and reached via their parent's childrenIds.
              includeInOverview: doc.isParent,
              parentId: doc.isParent ? null : doc.docName,
            },
            body: page.markdown,
            // The file URL is required, otherwise Starlight doesn't apply its
            // Markdown plugins and `:::note` etc. are not rendered as asides.
            rendered: await context.renderMarkdown(page.markdown, {
              fileURL: pathToFileURL(path.resolve(doc.docPath)),
            }),
          });
        }
      } catch (error) {
        console.error(`Error loading ${source.groupPrefix}:`, error);
        throw error;
      }
    },
  };
}

/**
 * Generate a content collection containing the release notes of each release as an entry.
 * This function parses the release-notes.txt file from the root of the RIOT repository.
 * The release-notes syntax is converted to correct markdown and then parsed.
 */
export function changelogLoader(): Loader {
  return {
    name: "changelog-loader",
    load: async (context): Promise<void> => {
      // Read release-notes.txt file
      const buffer = await fs.readFile("../../release-notes.txt");
      // Convert the buffer to text
      const content = buffer.toString("utf8");
      // convert the text to an array of lines
      const lines = content.split("\n");

      let currentReleaseHeading: string | null = null;
      let currentReleaseHeadingIndex: number | null = null;

      for (const [index, line] of lines.entries()) {
        const nextLine = lines[index + 1];

        // Convert "Setext Style" headings to "ATX Style" headings
        // This is necessary to support heading nesting higher than h1 and h2
        // Check if next line contains only "=" characters
        if (/^=+$/.test(nextLine)) {
          // If yes add a "##" to the current lines beginning
          lines[index] = `# ${line}`;
          // Remove the next lines content
          lines[index + 1] = "";
        }
        // Check if next line contains only "-" characters
        if (/^-+$/.test(nextLine)) {
          // If yes add a "###" to the current lines beginning
          lines[index] = `## ${line}`;
          // Remove the next lines content
          lines[index + 1] = "";
        }

        // Convert all headings to be one level lower
        // h1 => h2, h2 => h3, h3 => h4 etc.
        // This is necessary, because the heading levels of this file are wrong (for markdown)
        // We cannot have more than one h1 per document etc.
        if (lines[index].startsWith("#")) {
          lines[index] = `#${lines[index]}`;
        }

        // Check if the current line is a RIOT release heading
        // or if we have reached the last line of the file.
        if (
          /(RIOT-.*? - Release Notes)|(Release 2013\.08)/.test(line) ||
          index == lines.length - 1
        ) {
          // If we already found a release heading before, we now
          // reached the end of its changelog
          if (
            currentReleaseHeading != null &&
            currentReleaseHeadingIndex != null
          ) {
            // Extract the release (YYYY.MM.*) from the release heading
            const release =
              currentReleaseHeading.match(/(\d{4}\.\d{2}(\.\d+)?)/)?.[1] || "";

            // Create a slug based on the date
            const slug = release.replaceAll(".", "-");

            // Generate a date object from the slug (since it is a date)
            const date = new Date(release);

            // Extract the release code name from the heading if it exists
            const codeName = currentReleaseHeading.match(/"([^"]+)"/)?.[1] ||
              null;

            // Extract current release content
            const currentReleaseContent = lines
              .slice(currentReleaseHeadingIndex + 1, index)
              .join("\n");

            // Replace #ID entries with links to GitHub issues
            // Github treats both PRs and Issues the same way regarding links
            const contentWithGithubLinks = currentReleaseContent.replace(
              /#(\d+)/g,
              `[#$1](${REPOSITORY_URL}/issues/$1)`,
            );

            // Add entry to content collection
            context.store.set({
              id: slug,
              data: {
                title: `RIOT-${release}${
                  codeName ? ` "${codeName}"` : ""
                } - Release Notes`,
                release: release,
                codeName: codeName,
                slug: slug,
                date: date,
                markdown: contentWithGithubLinks,
              },
              rendered: await context.renderMarkdown(contentWithGithubLinks),
            });
          }

          currentReleaseHeading = line;
          currentReleaseHeadingIndex = index;
        }
      }
    },
  };
}
