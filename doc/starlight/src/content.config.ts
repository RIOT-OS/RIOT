import { defineCollection, z } from "astro:content";
import { type Loader } from "astro/loaders";
import { docsLoader } from "@astrojs/starlight/loaders";
import { docsSchema } from "@astrojs/starlight/schema";
import { promises as fs } from "node:fs";

export const collections = {
  docs: defineCollection({ loader: docsLoader(), schema: docsSchema() }),
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
                markdown: currentReleaseContent,
              },
              rendered: await context.renderMarkdown(currentReleaseContent),
            });
          }

          currentReleaseHeading = line;
          currentReleaseHeadingIndex = index;
        }
      }
    },
  };
}
