/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/** Lower-case words without punctuation, e.g. `ESP32-S3 Box` → `esp32s3 box`. */
export function normalizeSearchText(text: string): string {
  return text
    .toLowerCase()
    .replace(/[^\p{L}\p{N}\s]/gu, "")
    .replace(/\s+/g, " ")
    .trim();
}

/** True if each word of `query` is in the normalized `text`, e.g. `nucleo f4` in `nucleof446re`. */
export function matchesQuery(text: string, query: string): boolean {
  return normalizeSearchText(query)
    .split(" ")
    .every((word) => text.includes(word));
}
