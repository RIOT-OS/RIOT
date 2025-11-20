import { defineRouteMiddleware } from "@astrojs/starlight/route-data";

/**
 * Since we currently use a symlink to comply with the (broken)
 * docs folder scanning of starlight, this will break the edit
 * link since, while technically correct, our symlink will be
 * shown as github.com/RIOT-OS/RIOT/doc/guides/src/content/docs/foobar
 * Since github can't parse symlinks, we need to modify the editUrl
 * through a route middleware in order for the edit link to still function
 *
 * As soon as base directory changes get fixed, this should be removed
 * again! See: https://github.com/withastro/starlight/pull/3332
 */
export const onRequest = defineRouteMiddleware((context) => {
  const { starlightRoute } = context.locals;

  if (starlightRoute.editUrl) {
    starlightRoute.editUrl = new URL(
      starlightRoute.editUrl.href.replace("src/content/docs/", ""),
      starlightRoute.editUrl,
    );
  }
});
