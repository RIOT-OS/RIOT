/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/** The Markdown processor of the site, for `astro.config.ts` and the tests. */

import { satteri, type SatteriProcessorOptions } from "@astrojs/markdown-satteri";
import * as emoji from "node-emoji";

type SatteriMdastPlugin = Extract<
  NonNullable<SatteriProcessorOptions["mdastPlugins"]>[number],
  { name: string }
>;

function satteriEmoji(): SatteriMdastPlugin {
  return {
    name: "satteri-emoji",
    text(node, ctx) {
      if (node.value && node.value.includes(":")) {
        const emojified = emoji.emojify(node.value);
        if (emojified !== node.value) {
          ctx.setProperty(node, "value", emojified);
        }
      }
    },
  };
}

export const markdownProcessor = satteri({
  mdastPlugins: [satteriEmoji],
});
