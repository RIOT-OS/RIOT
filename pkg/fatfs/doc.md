/**
 * @defgroup pkg_fatfs   FAT file system
 * @ingroup  pkg
 * @ingroup  sys_fs
 * @brief    Provides FAT file system support
 * @see      http://elm-chan.org/fsw/ff/00index_e.html
 */

# Compile-Time options

 - `FATFS_FFCONF_OPT_FS_READONLY`: This option switches read-only configuration.
                                   Read-only configuration removes writing API functions.

 - `FATFS_FFCONF_OPT_USE_FASTSEEK`: Enables faster seek at the cost of some per-fd RAM.

 - `FATFS_FFCONF_OPT_FS_TINY`: Memory optimisations. Disabling this will increase FS performance
                               at the cost of higher per-FD RAM usage (+512 byte/fd) as well as
                               increased stack usage.

 - `FATFS_FFCONF_OPT_FS_EXFAT`: enables support for exFAT

 - `FATFS_FFCONF_OPT_USE_LFN`: enables support for long file names
