// @ts-check
import { defineConfig } from "astro/config";
import starlight from "@astrojs/starlight";

// https://astro.build/config
export default defineConfig({
  site: "https://guide.riot-os.org",
  integrations: [
    starlight({
      title: "RIOT Documentation",
      social: [
        {
          icon: "github",
          label: "GitHub",
          href: "https://github.com/riot-os/riot",
        },
        {
          icon: "mastodon",
          label: "Mastodon",
          href: "https://floss.social/@RIOT_OS",
        },
        {
          icon: "blueSky",
          label: "Bluesky",
          href: "https://bsky.app/profile/riot-os.org",
        },
        {
          icon: "youtube",
          label: "YouTube",
          href: "https://www.youtube.com/c/RIOT-IoT",
        },
        {
          icon: "discourse",
          label: "Forum",
          href: "https://forum.riot-os.org",
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
          label: "Code of Conduct",
          collapsed: true,
          items: [
            "general/code_of_conduct",
            "general/code_of_conduct/faq",
            "general/code_of_conduct/reporting",
          ],
        },
        {
          label: "RIOT in a Nutshell",
          items: [
            { label: "Introduction", slug: "index" },
            "general/structure",
            "general/vision",
            "general/governance",
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
            {
              label: "C Basics",
              items: [
                "c_tutorials/create_project",
                "c_tutorials/timers",
                "c_tutorials/shell",
                "c_tutorials/threads",
                "c_tutorials/gpio",
                "c_tutorials/saul",
                "c_tutorials/coding_conventions",
              ],
            },
            {
              label: "C++ Basics",
              items: [
                "cpp_tutorials/using_cpp",
                "cpp_tutorials/coding_conventions",
              ],
            },
            {
              label: "Rust Basics",
              items: [
                "rust_tutorials/rust_in_riot",
                "rust_tutorials/create_project",
              ],
            },
            {
              label: "Advanced",
              items: [
                "advanced_tutorials/creating_application",
                "advanced_tutorials/creating_modules",
                "advanced_tutorials/device_drivers",
                "advanced_tutorials/porting_boards",
              ],
            },
          ],
        },
        {
          label: "Build System",
          items: [
            "build-system/build_system",
            "build-system/build_system_basics",
            "build-system/kconfig",
            "build-system/flashing",
            "build-system/build-in-docker",
            "build-system/advanced_build_system_tricks",
            "build-system/debugging_aids",
          ],
        },
        {
          label: "Miscellaneous",
          items: [
            "misc/dev_best_practices",
            "misc/io_mapping_and_shields",
            "misc/roadmap",
            "misc/lost_and_found",
            "misc/release_cycle",
            "misc/emulators",
            "misc/terminal_config",
            "misc/how_to_doc",
          ],
        },
      ],
      customCss: ["./src/styles/custom.css", "./src/fonts/font-face.css"],
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
  vite: {
    server: {
      fs: {
        allow: ["./", "../doxygen"],
      },
    },
  },
});
