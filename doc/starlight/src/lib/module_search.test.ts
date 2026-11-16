/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Tests of the filter of the board and CPU overview, see `module_search.ts`.
 * Run with `make test`.
 */

import { test } from "node:test";
import assert from "node:assert/strict";
import { matchesQuery, normalizeSearchText } from "./module_search.ts";

test("the search text has no case, punctuation or repeated spaces", () => {
  assert.equal(
    normalizeSearchText("BBC micro:bit v2  esp32-s3_wroom aconno™ CC26x2, CC13x2"),
    "bbc microbit v2 esp32s3wroom aconno cc26x2 cc13x2",
  );
});

test("a query matches if each of its words is in the text", () => {
  const text = normalizeSearchText(
    "STM32 Nucleo-F446RE nucleo-f446re Support for the STM32 Nucleo-F446RE",
  );
  for (const query of ["", "  ", "nucleo", "Nucleo F446", "nucleo-f4", "support STM32"]) {
    assert.ok(matchesQuery(text, query), query);
  }
  for (const query of ["nucleo f7", "risc-v", "stm32 nucleo l4"]) {
    assert.ok(!matchesQuery(text, query), query);
  }
});

test("punctuation in the query is optional", () => {
  const text = normalizeSearchText("GD32V gd32v GigaDevice GD32 RISC-V Microcontrollers");
  for (const query of ["risc-v", "RISCV", "risc v", "gd32-v"]) {
    assert.ok(matchesQuery(text, query), query);
  }
});
