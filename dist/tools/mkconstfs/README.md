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

# mkconstfs2

This is an alternative tool that takes a list of files instead of a whole
directory.

File generation can be customized via a INI file containing template
fragments. Note that the INI parser has comments disabled (to allow for
literal ";" and "#" characters). Also, leading whitespace is removed,
requiring the use of escape sequences.
