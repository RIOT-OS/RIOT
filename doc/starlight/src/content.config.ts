/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

import { defineCollection } from "astro:content";
import { type Loader } from "astro/loaders";
import { docsSchema } from "@astrojs/starlight/schema";
import { promises as fs } from "node:fs";
import { glob } from "astro/loaders";
import { z } from "astro/zod";
import path from "node:path";
import {
  extractBoardTitleFromDoxygen,
  transformDoxygenMarkdown,
} from "./lib/doxygen_filter";

export const collections = {
  docs: defineCollection({
    loader: glob({ pattern: "**/*.(md|mdx)", base: "../guides" }),
    schema: docsSchema(),
  }),
  boards: defineCollection({
    loader: boardsLoader(),
    schema: docsSchema(),
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

/**
 * Load board documentation from doc.md files in the boards directory.
 * Each board directory should contain a doc.md file with markdown content.
 * Extracts board titles from @defgroup directives like riot-index does.
 */
export function boardsLoader(): Loader {
  return {
    name: "boards-loader",
    load: async (context): Promise<void> => {
      const boardsDir = "../../boards";

      try {
        // Read all directories in the boards folder
        const entries = await fs.readdir(boardsDir, { withFileTypes: true });
        const boardDirs = entries.filter((entry) => entry.isDirectory());

        for (const boardDir of boardDirs) {
          const boardName = boardDir.name;
          const docPath = path.join(boardsDir, boardName, "doc.md");

          // Check if doc.md exists for this board
          try {
            const docContent = await fs.readFile(docPath, "utf-8");

            // Parse frontmatter out of the markdown content if it exists
            let markdown = docContent;
            const fallbackTitle = boardName.replace(/-/g, " ");
            const title = extractBoardTitleFromDoxygen(
              docContent,
              fallbackTitle,
            );
            const filteredMarkdown = transformDoxygenMarkdown(markdown);

            // Parse @brief directives for description (take the first one as description)
            const descriptionMatch = docContent.match(/@brief\s+(.+)/);
            const description = descriptionMatch
              ? descriptionMatch[1].trim()
              : "";

            // Create entry ID
            const entryId = `boards/${boardName}`;

            // Add entry to content collection
            context.store.set({
              id: entryId,
              data: {
                title: title,
                description: description,
              },
              body: filteredMarkdown,
              rendered: await context.renderMarkdown(filteredMarkdown),
            });
          } catch (error) {
            console.debug(
              `Tried to load documentation for board "${boardName}" but it doesn't contain a doc.md file, skipping...`,
            );
            continue;
          }
        }
      } catch (error) {
        console.error("Error loading boards:", error);
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
            const codeName =
              currentReleaseHeading.match(/"([^"]+)"/)?.[1] || null;

            // Extract current release content
            const currentReleaseContent = lines
              .slice(currentReleaseHeadingIndex + 1, index)
              .join("\n");

            // Replace #ID entries with links to GitHub issues
            // Github treats both PRs and Issues the same way regarding links
            const contentWithGithubLinks = currentReleaseContent.replace(
              /#(\d+)/g,
              "[#$1](https://github.com/RIOT-OS/RIOT/issues/$1)",
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
