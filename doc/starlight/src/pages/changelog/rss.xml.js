import rss from "@astrojs/rss";
import { getCollection } from "astro:content";

export async function GET(context) {
  const changelog = await getCollection("changelog");

  return rss({
    // `<title>` field in output xml
    title: "RIOT Changelog",
    // `<description>` field in output xml
    description: "Changelog of RIOT releases",
    // Pull in your project "site" from the endpoint context
    // https://docs.astro.build/en/reference/api-reference/#site
    site: context.site,
    // Array of `<item>`s in output xml
    // See "Generating items" section for examples using content collections and glob imports
    items: changelog.map((entry) => ({
      title: entry.data.title,
      link: `/changelog/${entry.data.slug}`,
      pubDate: entry.data.date,
    })),
  });
}
