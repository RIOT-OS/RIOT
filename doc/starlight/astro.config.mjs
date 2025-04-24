// @ts-check
import { defineConfig } from "astro/config";
import starlight from "@astrojs/starlight";

// https://astro.build/config
export default defineConfig({
  site: "https://guide.riot-os.org",
  integrations: [
    starlight({
      title: "Riot Documentation",
      social: [
        {
          icon: "github",
          label: "GitHub",
          href: "https://github.com/riot-os/riot",
        },
        {
          icon: "mastodon",
          label: "Mastodon",
          href: "https://fosstodon.org/@RIOT_OS",
        },
        {
          icon: "matrix",
          label: "Matrix",
          href: "https://matrix.to/#/#riot-os:matrix.org",
        },
      ],
      sidebar: [
        {
          label: "External Documentation",
          items: [
            {
              label: "API Documentation",
              link: "https://doc.riot-os.org",
              badge: { text: "Doxygen", variant: "tip" },
            },
            {
              label: "Rust Documentation",
              link: "https://doc.riot-os.org/rustdoc/latest/riot_doc_helpers/",
              badge: { text: "Rustdoc", variant: "tip" },
            },
          ],
        },
        {
          label: "RIOT in a Nutshell",
          items: [
            { label: "Introduction", slug: "index" },
            "general/structure",
          ],
        },
        {
          label: "Tutorials",
          items: [
            {
              label: "Getting Started",
              items: [
                "getting-started/install-wsl",
                "getting-started/installing",
                "getting-started/flashing",
                "getting-started/building_example",
                "getting-started/finding_modules",
              ],
            },
          ],
        },
        {
          label: "Build System",
          items: [
            "build-system/build_system",
            "build-system/build_system_basics",
            "build-system/build-in-docker",
            "build-system/advanced_build_system_tricks",
          ],
        },
        {
          label: "Miscellaneous",
          items: ["misc/how_to_doc"],
        },
      ],
      customCss: [],
      logo: {
        src: "./src/assets/riot-logo.svg",
        replacesTitle: true,
      },
      plugins: [],
      editLink: {
        baseUrl: "https://github.com/RIOT-OS/RIOT/tree/master/doc/guides",
      },
    }),
  ],
});
