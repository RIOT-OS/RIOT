/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

import type { APIRoute, GetStaticPaths } from "astro";
import { promises as fs } from "node:fs";
import { lookup } from "mrmime";
import path from "node:path";

/**
 * Directories in this repo that contain images referenced by some markdown files.
 */
const IMAGE_DIRS = ["../doxygen/src/pinouts"];

/**
 * Generate paths for each image from the specified dirs.
 */
export const getStaticPaths: GetStaticPaths = async () => {
  const pathsPerDir = await Promise.all(
    IMAGE_DIRS.map(async (dir) =>
      (await fs.readdir(dir, { withFileTypes: true }))
        .filter((entry) => entry.isFile())
        .map((entry) => ({
          params: { image: entry.name },
          props: { filePath: path.join(dir, entry.name) },
        })),
    ),
  );

  return pathsPerDir.flat();
};

/**
 * API route responding with the correct image.
 */
export const GET: APIRoute = async ({ props }) => {
  return new Response(await fs.readFile(props.filePath), {
    headers: {
      "Content-Type": lookup(props.filePath) ?? "application/octet-stream",
    },
  });
};
