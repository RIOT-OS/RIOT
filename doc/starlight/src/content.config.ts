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

export function changelogLoader(): Loader {
  return {
    name: "changelog-loader",
    load: async (context): Promise<void> => {
      const buffer = await fs.readFile("../../release-notes.txt");
      const fileContent = buffer.toString("utf8"); // or just buffer.toString()
      const lines = fileContent.split("\n");

      const releaseHeadings = lines.filter((line) =>
        /RIOT-.*? - Release Notes/.test(line)
      );

      context.store.clear();

      for (const [index, heading] of releaseHeadings.entries()) {
        const text = fileContent;

        const currentHeading = heading;
        const nextHeading = releaseHeadings[index + 1];

        const startIndex = text.indexOf(currentHeading);
        const endIndex = text.indexOf(nextHeading);

        const slug = (heading.match(/RIOT-([\d.]+)/)?.[1] || "").replaceAll(
          ".",
          "-"
        );

        // Extract current release markdown
        // and remove h1 heading (frontmatter already specifies the heading)
        const markdown = text
          .substring(startIndex, endIndex)
          .trim()
          .split("\n")
          .slice(2)
          .join("\n");

        context.store.set({
          id: heading,
          data: {
            title: heading,
            slug: slug,
            markdown: markdown,
          },
          rendered: await context.renderMarkdown(markdown),
        });
      }
    },
  };
}
