/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Peter Andersson (pelleplutt1976<at>gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

/*
 * spiffs_config.h
 *
 *  Created on: Jul 3, 2013
 *      Author: petera
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

// ----------- 8< ------------
// Following includes are for the linux test build of spiffs
// These may/should/must be removed/altered/replaced in your target
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>

// ----------- >8 ------------

// compile time switches

// Set generic spiffs debug output call.
#ifndef SPIFFS_DBG
#define SPIFFS_DBG(...) //printf(_f, ## __VA_ARGS__)
#endif
// Set spiffs debug output call for garbage collecting.
#ifndef SPIFFS_GC_DBG
#define SPIFFS_GC_DBG(...) //printf(_f, ## __VA_ARGS__)
#endif
// Set spiffs debug output call for caching.
#ifndef SPIFFS_CACHE_DBG
#define SPIFFS_CACHE_DBG(...) //printf(_f, ## __VA_ARGS__)
#endif
// Set spiffs debug output call for system consistency checks.
#ifndef SPIFFS_CHECK_DBG
#define SPIFFS_CHECK_DBG(...) //printf(_f, ## __VA_ARGS__)
#endif
// Set spiffs debug output call for all api invocations.
#ifndef SPIFFS_API_DBG
#define SPIFFS_API_DBG(...) //printf(_f, ## __VA_ARGS__)
#endif

// Defines spiffs debug print formatters
// some general signed number
#ifndef _SPIPRIi
#define _SPIPRIi   "%d"
#endif
// address
#ifndef _SPIPRIad
#define _SPIPRIad  "%08x"
#endif
// block
#ifndef _SPIPRIbl
#define _SPIPRIbl  "%04x"
#endif
// page
#ifndef _SPIPRIpg
#define _SPIPRIpg  "%04x"
#endif
// span index
#ifndef _SPIPRIsp
#define _SPIPRIsp  "%04x"
#endif
// file descriptor
#ifndef _SPIPRIfd
#define _SPIPRIfd  "%d"
#endif
// file object id
#ifndef _SPIPRIid
#define _SPIPRIid  "%04x"
#endif
// file flags
#ifndef _SPIPRIfl
#define _SPIPRIfl  "%02x"
#endif

// Enable/disable API functions to determine exact number of bytes
// for filedescriptor and cache buffers. Once decided for a configuration,
// this can be disabled to reduce flash.
#ifndef SPIFFS_BUFFER_HELP
#define SPIFFS_BUFFER_HELP              0
#endif

// Enables/disable memory read caching of nucleus file system operations.
// If enabled, memory area must be provided for cache in SPIFFS_mount.
#ifndef  SPIFFS_CACHE
#define SPIFFS_CACHE                    1
#endif
#if SPIFFS_CACHE
// Enables memory write caching for file descriptors in hydrogen
#ifndef  SPIFFS_CACHE_WR
#define SPIFFS_CACHE_WR                 1
#endif

// Enable/disable statistics on caching. Debug/test purpose only.
#ifndef  SPIFFS_CACHE_STATS
#define SPIFFS_CACHE_STATS              1
#endif
#endif

// Always check header of each accessed page to ensure consistent state.
// If enabled it will increase number of reads, will increase flash.
#ifndef SPIFFS_PAGE_CHECK
#define SPIFFS_PAGE_CHECK               1
#endif

// Define maximum number of gc runs to perform to reach desired free pages.
#ifndef SPIFFS_GC_MAX_RUNS
#define SPIFFS_GC_MAX_RUNS              5
#endif

// Enable/disable statistics on gc. Debug/test purpose only.
#ifndef SPIFFS_GC_STATS
#define SPIFFS_GC_STATS                 1
#endif

// Garbage collecting examines all pages in a block which and sums up
// to a block score. Deleted pages normally gives positive score and
// used pages normally gives a negative score (as these must be moved).
// To have a fair wear-leveling, the erase age is also included in score,
// whose factor normally is the most positive.
// The larger the score, the more likely it is that the block will
// picked for garbage collection.

// Garbage collecting heuristics - weight used for deleted pages.
#ifndef SPIFFS_GC_HEUR_W_DELET
#define SPIFFS_GC_HEUR_W_DELET          (5)
#endif
// Garbage collecting heuristics - weight used for used pages.
#ifndef SPIFFS_GC_HEUR_W_USED
#define SPIFFS_GC_HEUR_W_USED           (-1)
#endif
// Garbage collecting heuristics - weight used for time between
// last erased and erase of this block.
#ifndef SPIFFS_GC_HEUR_W_ERASE_AGE
#define SPIFFS_GC_HEUR_W_ERASE_AGE      (50)
#endif

// Object name maximum length. Note that this length include the
// zero-termination character, meaning maximum string of characters
// can at most be SPIFFS_OBJ_NAME_LEN - 1.
#ifndef SPIFFS_OBJ_NAME_LEN
#define SPIFFS_OBJ_NAME_LEN             (32)
#endif

// Maximum length of the metadata associated with an object.
// Setting to non-zero value enables metadata-related API but also
// changes the on-disk format, so the change is not backward-compatible.
//
// Do note: the meta length must never exceed
// logical_page_size - (SPIFFS_OBJ_NAME_LEN + 64)
//
// This is derived from following:
// logical_page_size - (SPIFFS_OBJ_NAME_LEN + sizeof(spiffs_page_header) +
// spiffs_object_ix_header fields + at least some LUT entries)
#ifndef SPIFFS_OBJ_META_LEN
#define SPIFFS_OBJ_META_LEN             (0)
#endif

// Size of buffer allocated on stack used when copying data.
// Lower value generates more read/writes. No meaning having it bigger
// than logical page size.
#ifndef SPIFFS_COPY_BUFFER_STACK
#define SPIFFS_COPY_BUFFER_STACK        (64)
#endif

// Enable this to have an identifiable spiffs filesystem. This will look for
// a magic in all sectors to determine if this is a valid spiffs system or
// not on mount point. If not, SPIFFS_format must be called prior to mounting
// again.
#ifndef SPIFFS_USE_MAGIC
#define SPIFFS_USE_MAGIC                (1)
#endif

#if SPIFFS_USE_MAGIC
// Only valid when SPIFFS_USE_MAGIC is enabled. If SPIFFS_USE_MAGIC_LENGTH is
// enabled, the magic will also be dependent on the length of the filesystem.
// For example, a filesystem configured and formatted for 4 megabytes will not
// be accepted for mounting with a configuration defining the filesystem as 2
// megabytes.
#ifndef SPIFFS_USE_MAGIC_LENGTH
#define SPIFFS_USE_MAGIC_LENGTH         (0)
#endif
#endif

// SPIFFS_LOCK and SPIFFS_UNLOCK protects spiffs from reentrancy on api level
// These should be defined on a multithreaded system

struct spiffs_t;
void spiffs_lock(struct spiffs_t *fs);
void spiffs_unlock(struct spiffs_t *fs);

// define this to enter a mutex if you're running on a multithreaded system
#ifndef SPIFFS_LOCK
#define SPIFFS_LOCK(fs) spiffs_lock(fs)
#endif
// define this to exit a mutex if you're running on a multithreaded system
#ifndef SPIFFS_UNLOCK
#define SPIFFS_UNLOCK(fs) spiffs_unlock(fs)
#endif

// Enable if only one spiffs instance with constant configuration will exist
// on the target. This will reduce calculations, flash and memory accesses.
// Parts of configuration must be defined below instead of at time of mount.
#ifndef SPIFFS_SINGLETON
#define SPIFFS_SINGLETON 0
#endif

#if SPIFFS_SINGLETON
// Instead of giving parameters in config struct, singleton build must
// give parameters in defines below.
#ifndef SPIFFS_CFG_PHYS_SZ
#define SPIFFS_CFG_PHYS_SZ(ignore)        (1024*1024*2)
#endif
#ifndef SPIFFS_CFG_PHYS_ERASE_SZ
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (65536)
#endif
#ifndef SPIFFS_CFG_PHYS_ADDR
#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)
#endif
#ifndef SPIFFS_CFG_LOG_PAGE_SZ
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)
#endif
#ifndef SPIFFS_CFG_LOG_BLOCK_SZ
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (65536)
#endif
#endif

// Enable this if your target needs aligned data for index tables
#ifndef SPIFFS_ALIGNED_OBJECT_INDEX_TABLES
#define SPIFFS_ALIGNED_OBJECT_INDEX_TABLES       1
#endif

// Enable this if you want the HAL callbacks to be called with the spiffs struct
#ifndef SPIFFS_HAL_CALLBACK_EXTRA
#define SPIFFS_HAL_CALLBACK_EXTRA         1
#endif

// Enable this if you want to add an integer offset to all file handles
// (spiffs_file). This is useful if running multiple instances of spiffs on
// same target, in order to recognise to what spiffs instance a file handle
// belongs.
// NB: This adds config field fh_ix_offset in the configuration struct when
// mounting, which must be defined.
#ifndef SPIFFS_FILEHDL_OFFSET
#define SPIFFS_FILEHDL_OFFSET                 0
#endif

// Enable this to compile a read only version of spiffs.
// This will reduce binary size of spiffs. All code comprising modification
// of the file system will not be compiled. Some config will be ignored.
// HAL functions for erasing and writing to spi-flash may be null. Cache
// can be disabled for even further binary size reduction (and ram savings).
// Functions modifying the fs will return SPIFFS_ERR_RO_NOT_IMPL.
// If the file system cannot be mounted due to aborted erase operation and
// SPIFFS_USE_MAGIC is enabled, SPIFFS_ERR_RO_ABORTED_OPERATION will be
// returned.
// Might be useful for e.g. bootloaders and such.
#ifndef SPIFFS_READ_ONLY
#define SPIFFS_READ_ONLY                      0
#endif

// Set SPIFFS_TEST_VISUALISATION to non-zero to enable SPIFFS_vis function
// in the api. This function will visualize all filesystem using given printf
// function.
#ifndef SPIFFS_TEST_VISUALISATION
#define SPIFFS_TEST_VISUALISATION         0
#endif
#if SPIFFS_TEST_VISUALISATION
#ifndef spiffs_printf
#define spiffs_printf(...)                DEBUG(__VA_ARGS__)
#endif
// spiffs_printf argument for a free page
#ifndef SPIFFS_TEST_VIS_FREE_STR
#define SPIFFS_TEST_VIS_FREE_STR          "_"
#endif
// spiffs_printf argument for a deleted page
#ifndef SPIFFS_TEST_VIS_DELE_STR
#define SPIFFS_TEST_VIS_DELE_STR          "/"
#endif
// spiffs_printf argument for an index page for given object id
#ifndef SPIFFS_TEST_VIS_INDX_STR
#define SPIFFS_TEST_VIS_INDX_STR(id)      "i"
#endif
// spiffs_printf argument for a data page for given object id
#ifndef SPIFFS_TEST_VIS_DATA_STR
#define SPIFFS_TEST_VIS_DATA_STR(id)      "d"
#endif
#endif

// Types depending on configuration such as the amount of flash bytes
// given to spiffs file system in total (spiffs_file_system_size),
// the logical block size (log_block_size), and the logical page size
// (log_page_size)

// Block index type. Make sure the size of this type can hold
// the highest number of all blocks - i.e. spiffs_file_system_size / log_block_size
typedef uint16_t spiffs_block_ix;
// Page index type. Make sure the size of this type can hold
// the highest page number of all pages - i.e. spiffs_file_system_size / log_page_size
typedef uint16_t spiffs_page_ix;
// Object id type - most significant bit is reserved for index flag. Make sure the
// size of this type can hold the highest object id on a full system,
// i.e. 2 + (spiffs_file_system_size / (2*log_page_size))*2
typedef uint16_t spiffs_obj_id;
// Object span index type. Make sure the size of this type can
// hold the largest possible span index on the system -
// i.e. (spiffs_file_system_size / log_page_size) - 1
typedef uint16_t spiffs_span_ix;

typedef uint8_t u8_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;

#ifdef __cplusplus
}
#endif
