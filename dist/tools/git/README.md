# Overview

This directory contains some git tools used by RIOT's build system

## git-cache

Simple git caching script, from https://github.com/kaspar030/git-cache
If git-cache is unconfigured, the script pulls from the given remote location.

In order to set up the cache, do:

- install the git-cache binary into path.
  This will make the script available as "git cache ...".
  Alternatively, directly execute it.
- run "git cache init", which initializes a git cache in ${HOME}/.gitcache.
  The used path can be overridden using the "GIT_CACHE_DIR" environment
  variable.
  The cache repository will be used to cache multiple remote repositories.
- add a repository to the cache: "git cache add \<name\> \<URL\>
- whenever needed (at least once after adding a repository),
  run "git cache update"
