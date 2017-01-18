# Overview

This directory contains some git tools used by RIOT's build system

## git-cache

Simple git caching script, from https://github.com/kaspar030/git-cache
If git-cache has not been initialized, it will fall back to do checkouts from
the remote repository.
Otherwise it will transparently cache repositories in it's cache folder,
ideally not touching the network if a requested commit / tag can be cloned from
the cache.

In order to set up the cache, do:

- install the git-cache binary into path.
  This will make the script available as "git cache ...".
  Alternatively, directly execute it.
- run "git cache init" (or "dist/tools/git/git-cache init", which initializes a
  git cache in ${HOME}/.gitcache.
  The used path can be overridden using the "GIT_CACHE_DIR" environment
  variable.
  The cache repository will be used to cache multiple remote repositories.
