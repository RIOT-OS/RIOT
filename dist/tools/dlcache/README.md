# Introduction

This script aims to cache http(s)-downloads

## How it works

- if a file with the right name and md5 exists, nothing happens
- if a file with the right name and md5 is in cache, use that
- if a file with the right name but wrong md5 is in cache, redownload
  download to cache
- after download, check md5, then copy to target
