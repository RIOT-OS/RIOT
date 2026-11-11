/*
 * SPDX-FileCopyrightText: 2026 Lasse Rosenow <Lasse.Rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Tests of the tag file reader. Run with `npm test`.
 */

import { test } from "node:test";
import assert from "node:assert/strict";
import { promises as fs } from "node:fs";
import { createServer, type IncomingMessage, type ServerResponse } from "node:http";
import type { AddressInfo } from "node:net";
import os from "node:os";
import path from "node:path";
import { loadDoxygenTagFile, parseDoxygenTagFile } from "./doxygen_tagfile.ts";

/** A file compound that documents the macro `name`. */
function fileWithDefine(path: string, name: string, anchor: string): string {
  const file = `${path.replaceAll("/", "_2")}board_8h.html`;
  return `  <compound kind="file">
    <name>board.h</name>
    <path>${path}</path>
    <filename>${file}</filename>
    <member kind="define">
      <type>#define</type>
      <name>${name}</name>
      <anchorfile>${file}</anchorfile>
      <anchor>${anchor}</anchor>
      <arglist></arglist>
    </member>
  </compound>`;
}

function tagFile(...compounds: string[]): string {
  return `<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.14.0">
${compounds.join("\n")}
</tagfile>
`;
}

const GENERIC_GPIO = `  <compound kind="group">
    <name>drivers_periph_gpio</name>
    <title>GPIO</title>
    <filename>group__drivers__periph__gpio.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>GPIO_PIN</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>generic</anchor>
      <arglist>(x, y)</arglist>
    </member>
  </compound>`;

test("groups resolve to their page and title", () => {
  const api = parseDoxygenTagFile(tagFile(GENERIC_GPIO));
  assert.deepEqual(api.lookup("drivers_periph_gpio"), {
    url: "group__drivers__periph__gpio.html",
    title: "GPIO",
    symbol: false,
  });
});

test("a macro is told apart from the other symbols", () => {
  const api = parseDoxygenTagFile(
    tagFile(
      GENERIC_GPIO,
      fileWithDefine("cpu/esp32/include/", "GPIO_PIN", "esp32"),
      `  <compound kind="file">
    <name>gpio.h</name>
    <path>drivers/include/periph/</path>
    <filename>periph_2gpio_8h.html</filename>
    <member kind="function">
      <type>int</type>
      <name>gpio_init</name>
      <anchorfile>group__drivers__periph__gpio.html</anchorfile>
      <anchor>init</anchor>
      <arglist>(gpio_t pin, gpio_mode_t mode)</arglist>
    </member>
  </compound>`,
    ),
  );
  assert.equal(api.lookup("gpio_init")?.macro, false);
  assert.equal(api.lookup("GPIO_PIN")?.macro, true);
  assert.equal(api.lookup("GPIO_PIN", "cpu/esp32/doc.md")?.macro, true);
  assert.equal(api.lookup("drivers_periph_gpio")?.macro, undefined);
});

test("a symbol defined once resolves from anywhere", () => {
  const api = parseDoxygenTagFile(tagFile(fileWithDefine("boards/foo/include/", "FOO_PIN", "a1")));
  assert.equal(api.lookup("FOO_PIN", "cpu/bar/doc.md")?.url, "boards_2foo_2include_2board_8h.html#a1");
});

test("a contested symbol resolves to the definition in the referrer's directory", () => {
  const api = parseDoxygenTagFile(
    tagFile(
      GENERIC_GPIO,
      fileWithDefine("cpu/esp32/include/", "GPIO_PIN", "esp32"),
      fileWithDefine("cpu/qn908x/include/", "GPIO_PIN", "qn908x"),
    ),
  );
  assert.match(api.lookup("GPIO_PIN", "cpu/esp32/doc.md")!.url, /#esp32$/);
  assert.match(api.lookup("GPIO_PIN", "cpu/qn908x/doc_foo.doc.md")!.url, /#qn908x$/);
  // a CPU without its own definition gets the generic API
  assert.match(api.lookup("GPIO_PIN", "cpu/gd32v/doc.md")!.url, /#generic$/);
  assert.match(api.lookup("GPIO_PIN")!.url, /#generic$/);
});

test("a definition in a sibling directory is not the referrer's own", () => {
  // `boards/common` is a category like `boards`: the `board.h` of one common
  // board group is no closer to another group than that of any other board
  const api = parseDoxygenTagFile(
    tagFile(
      fileWithDefine("boards/common/seeedstudio-xiao-nrf52840/include/", "LED0_PIN", "xiao"),
      fileWithDefine("boards/nucleo-f401re/include/", "LED0_PIN", "nucleo"),
    ),
  );
  assert.equal(api.lookup("LED0_PIN", "boards/common/nucleo64/doc.md"), undefined);
  assert.match(api.lookup("LED0_PIN", "boards/nucleo-f401re/doc.md")!.url, /#nucleo$/);
  assert.match(
    api.lookup("LED0_PIN", "boards/common/seeedstudio-xiao-nrf52840/doc.md")!.url,
    /#xiao$/,
  );
});

test("a contested symbol without a generic definition is not resolved", () => {
  const api = parseDoxygenTagFile(
    tagFile(
      fileWithDefine("boards/a/include/", "LED0_PIN", "a"),
      fileWithDefine("boards/b/include/", "LED0_PIN", "b"),
    ),
  );
  assert.equal(api.lookup("LED0_PIN"), undefined);
  assert.equal(api.lookup("LED0_PIN", "cpu/x/doc.md"), undefined);
});

test("an anchor in the documentation of a member resolves", () => {
  const api = parseDoxygenTagFile(
    tagFile(`  <compound kind="group">
    <name>sys_checksum_fletcher16</name>
    <title>Fletcher16</title>
    <filename>group__sys__checksum__fletcher16.html</filename>
    <member kind="function">
      <type>uint16_t</type>
      <name>fletcher16</name>
      <anchorfile>group__sys__checksum__fletcher16.html</anchorfile>
      <anchor>ga1</anchor>
      <arglist>(const uint8_t *buf, size_t bytes)</arglist>
      <docanchor file="group__sys__checksum__fletcher16.html">fletcher16_full</docanchor>
    </member>
    <docanchor file="group__sys__checksum__fletcher16.html" title="Usage">fletcher16_usage</docanchor>
  </compound>`),
  );
  assert.deepEqual(api.lookup("fletcher16_full"), {
    url: "group__sys__checksum__fletcher16.html#fletcher16_full",
    title: undefined,
    symbol: false,
  });
  assert.equal(api.lookup("fletcher16_usage")?.title, "Usage");
  assert.equal(api.lookup("fletcher16")?.url, "group__sys__checksum__fletcher16.html#ga1");
});

/* -------------------------------------------------------------------------- */
/* Loading                                                                    */
/* -------------------------------------------------------------------------- */

/** Run `run` with the path of a directory that is removed afterwards. */
async function withTempDir(run: (dir: string) => Promise<void>): Promise<void> {
  const dir = await fs.mkdtemp(path.join(os.tmpdir(), "doxygen-tagfile-"));
  try {
    await run(dir);
  } finally {
    await fs.rm(dir, { recursive: true, force: true });
  }
}

/** Run `run` with the URL of a local server that answers with `handler`. */
async function withServer(
  handler: (request: IncomingMessage, response: ServerResponse) => void,
  run: (url: string) => Promise<void>,
): Promise<void> {
  const server = createServer(handler);
  await new Promise<void>((resolve) => server.listen(0, "127.0.0.1", resolve));
  try {
    await run(`http://127.0.0.1:${(server.address() as AddressInfo).port}/riot.tag`);
  } finally {
    server.closeAllConnections();
    await new Promise((resolve) => server.close(resolve));
  }
}

test("a file that is not a complete tag file is not loaded", async () => {
  await withTempDir(async (dir) => {
    const file = path.join(dir, "riot.tag");
    const complete = tagFile(GENERIC_GPIO);

    await fs.writeFile(file, complete);
    assert.ok((await loadDoxygenTagFile(file)).lookup("drivers_periph_gpio"));

    // the error page of a server
    await fs.writeFile(file, "<html><body>503 Service Unavailable</body></html>\n");
    await assert.rejects(loadDoxygenTagFile(file), /not a complete Doxygen tag file/);
    // a file that is cut off
    await fs.writeFile(file, complete.slice(0, complete.indexOf("</compound>")));
    await assert.rejects(loadDoxygenTagFile(file), /not a complete Doxygen tag file/);
    await fs.writeFile(file, tagFile());
    await assert.rejects(loadDoxygenTagFile(file), /empty/);
  });
});

test("a download is cached and only repeated if the server has a newer file", async () => {
  let content = tagFile(GENERIC_GPIO);
  let etag = '"v1"';
  let downloads = 0;
  const handler = (request: IncomingMessage, response: ServerResponse) => {
    if (request.headers["if-none-match"] === etag) {
      response.writeHead(304).end();
      return;
    }
    downloads++;
    response.writeHead(200, { ETag: etag }).end(content);
  };

  await withTempDir((dir) =>
    withServer(handler, async (url) => {
      const cacheFile = path.join(dir, "cache", "riot.tag");
      for (let n = 0; n < 2; n++) {
        const api = await loadDoxygenTagFile(url, cacheFile);
        assert.ok(api.lookup("drivers_periph_gpio"));
        assert.equal(api.lookup("FOO_PIN"), undefined);
      }
      assert.equal(downloads, 1);

      content = tagFile(GENERIC_GPIO, fileWithDefine("boards/foo/include/", "FOO_PIN", "a1"));
      etag = '"v2"';
      for (let n = 0; n < 2; n++) {
        assert.ok((await loadDoxygenTagFile(url, cacheFile)).lookup("FOO_PIN"));
      }
      assert.equal(downloads, 2);

      // without a cache the file is downloaded every time
      assert.ok((await loadDoxygenTagFile(url)).lookup("FOO_PIN"));
      assert.equal(downloads, 3);
    }),
  );
});

test("the copy in the cache is used if the download fails", async () => {
  let respond = (response: ServerResponse) => {
    response.writeHead(200).end(tagFile(GENERIC_GPIO));
  };

  await withTempDir((dir) =>
    withServer(
      (_request, response) => respond(response),
      async (url) => {
        const cacheFile = path.join(dir, "riot.tag");
        const stale: string[] = [];
        const load = () => loadDoxygenTagFile(url, cacheFile, (reason) => stale.push(reason));

        // nothing in the cache yet
        respond = (response) => response.writeHead(500).end();
        await assert.rejects(load(), /HTTP 500/);

        respond = (response) => response.writeHead(200).end(tagFile(GENERIC_GPIO));
        assert.ok((await load()).lookup("drivers_periph_gpio"));
        assert.deepEqual(stale, []);

        respond = (response) => response.writeHead(500).end();
        assert.ok((await load()).lookup("drivers_periph_gpio"));
        // an error page that is sent with status 200 is not cached
        respond = (response) => response.writeHead(200).end("<html><body>Maintenance</body></html>");
        assert.ok((await load()).lookup("drivers_periph_gpio"));
        assert.ok((await load()).lookup("drivers_periph_gpio"));
        assert.equal(stale.length, 3);
        assert.match(stale[0], /HTTP 500/);
        assert.match(stale[1], /not a complete Doxygen tag file/);
      },
    ),
  );
});
