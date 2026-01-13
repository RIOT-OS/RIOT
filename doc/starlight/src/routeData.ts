import { defineRouteMiddleware } from "@astrojs/starlight/route-data";

const generatedPath = "/doc/guides/generated/";

/**
 * Since some of our guides are generated during build time, we need to
 * adjust the "Edit this page" links to point to the correct source files.
 * This middleware checks if the edit URL contains the generated path segment
 * and removes it to point to the actual source file.
 */
export const onRequest = defineRouteMiddleware((context) => {
  const { starlightRoute } = context.locals;

  if (starlightRoute.editUrl) {
    let editUrl = starlightRoute.editUrl.toString();
    if (editUrl.includes(generatedPath)) {
      const filename = editUrl.split(generatedPath)[1];
      editUrl = editUrl.split(generatedPath)[0] + "/" + filename;
    }
    starlightRoute.editUrl = new URL(editUrl);
  }
});
