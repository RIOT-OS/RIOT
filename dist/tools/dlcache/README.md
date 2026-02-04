# Introduction

This script aims to cache http(s)-downloads of (mainly) zip archives.

## How it works

- if a file with the right name and SHA512 exists, nothing happens
- if a file with the right name and SHA512 is in cache, use that
- if a file with the right name but wrong SHA512 is in cache, redownload
  download to cache
- after download, check SHA512, then copy to target
- if no SHA512 checksum is given, always redownload

## Usage

In order to stay compatible with the `$(DOWNLOAD_TO_FILE)` command,
the command usage was changed for Release 2025.10.

If no SHA512 checksum was given, the file will *always* be downloaded,
regardless of whether it is in the cache or not.

```sh
dist/tools/dlcache/dlcache.sh <output_folder> <URL> [SHA512 checksum]
```
