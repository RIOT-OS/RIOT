# Introduction

This tool creates a .c file including all data from a local directory as data
structures that can be mounted using constfs.

# Usage

    mkconstfs.py /path/to/files /

    #include "vfs.h"
    #include "fs/constfs.h"
    extern const vfs_mount_t _constfs;

    [...]

    vfs_mount((vfs_mount_t *)&_constfs);
