/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  fs
 * @{
 *
 * @file        scfs.h
 * @brief       A simple file system that emulates commands from the shell
 *              command module as binary files
 *
 * @author      Freie Universität Berlin
 * @author      Martine Lenders
 */

#ifndef __SCFS_H_
#define __SCFS_H_

#include <sys/stat.h>

/**
 * @brief Initializes SCFS.
 *
 * @param[in] mount_point   Path where to mount SCFS
 *
 * @return  0, on success
 * @return  other on error, errno is set accordingly
 */
int scfs_mount(const char *mount_point);

/**
 * @brief Removes SCFS.
 *
 * @param[in] mount_point   Path where SCFS is mounted
 *
 * @return  0, on success
 * @return  other on error, errno is set accordingly
 */
int scfs_unmount(const char *mount_point);

/**
 * @brief Get file status.
 *
 * @param[in] path  Path to the file.
 * @param[out] stat The status of the file.
 *
 * @return 0, on success.
 * @return -1, on failure. *errno* is set appropriatly.
 */
int scfs_stat(const char *path, struct stat *stat);

/**
 * @brief Executes a file.
 *
 * @param[in] path  Path to the file.
 * @param[in] argv  NULL-terminated array of command line arguments. By
 *                  convention, the first argument must the calling name of
 *                  the executed file.
 * @param[in] env   NULL-terminated array of strings, conventionally in the
 *                  form of "key=value", that represent the environment
 *                  variables passed to the command.
 *
 * @return  The return value of the executed program
 */
int scfs_exec(const char *path, const char *argv[], const char *env[]);

#endif /* __SCFS_H_ */
