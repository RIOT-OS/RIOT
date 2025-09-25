import { defineCollection } from "astro:content";
import { glob, type Loader } from "astro/loaders";
import { docsSchema } from "@astrojs/starlight/schema";
import { promises as fs } from "node:fs";

export const collections = {
  docs: defineCollection({
    loader: glob({ pattern: "**/*.(md|mdx)", base: "../guides" }),
    schema: docsSchema(),
  }),
  changelog: defineCollection({
    loader: changelogLoader(),
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

      // Convert all headings to be one level lower
      // h1 => h2, h2 => h3, h3 => h4
      // This is necessary, because the heading levels of this file are wrong (for markdown)
      // We cannot have more han one h1 per document etc.
      const fileContent = lines
        .map((line, index) => {
          const nextLine = lines[index + 1];

          // Check if next line contains only "=" characters
          if (/^=+$/.test(nextLine)) {
            // Remove the next lines content
            lines[index + 1] = "";
            // If yes add a "##" to the current lines beginning
            return `## ${line}`;
          }

          // Check if next line contains only "-" characters
          if (/^-+$/.test(nextLine)) {
            // Remove the next lines content
            lines[index + 1] = "";
            // If yes add a "###" to the current lines beginning
            return `### ${line}`;
          }

          // Otherwise just keep the line as is
          return line;
        })
        .join("\n");

      // /RIOT-.*? - Release Notes/
      const releaseHeadings = lines.filter((line) =>
        /(RIOT-.*? - Release Notes)|(Release 2013\.08)/.test(line)
      );

      // Iterate over all the release headings and extract the data to create entries for the collection
      for (const [index, heading] of releaseHeadings.entries()) {
        // Extract the release (YYYY.MM.*) from the heading
        const release = heading.match(/(\d{4}\.\d{2}(\.\d+)?)/)?.[1] || "";

        // Create a slug based on the date
        const slug = release.replaceAll(".", "-");

        // Generate a date object from the slug (since it is a date)
        const date = new Date(release);

        // Extract the release code name from the heading if it exists
        const codeName = heading.match(/"([^"]+)"/)?.[1] || null;

        // Extract current release content
        // Detect index of the heading of the current release in the release-notes.txt file
        const currentHeading = heading;
        const nextHeading = releaseHeadings[index + 1];

        // Detect index of the heading of the next release in the release-notes.txt file
        const startIndex = fileContent.indexOf(currentHeading);
        const endIndex = fileContent.indexOf(nextHeading);

        // Remove release heading from content (frontmatter already specifies the heading)
        const currentReleaseContent = fileContent
          .substring(startIndex, endIndex)
          .trim()
          .split("\n")
          .slice(2)
          .join("\n");

        // Add entry to content collection
        context.store.set({
          id: heading,
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
    },
  };
}
