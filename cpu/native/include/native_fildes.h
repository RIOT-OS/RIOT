/*
 * Copyright (C) 2014 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup native_fildes File Descriptors
 * @ingroup native_cpu
 *
 * @brief Native CPU File Descriptor API
 *
 * Common functionality for interacting with the host system via
 * file descriptors.
 *
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */


/**
 * Poll Handler Events
 *
 * definition of events that can be handled on file descriptors
 */
typedef enum {
    _ev_pollin = 0,     /**< see poll(3) */
    _ev_pollrdnorm,     /**< see poll(3) */
    _ev_pollrdband,     /**< see poll(3) */
    _ev_pollpri,        /**< see poll(3) */
    _ev_pollout,        /**< see poll(3) */
    _ev_pollwrband,     /**< see poll(3) */
    _ev_pollerr,        /**< see poll(3) */
    _ev_pollhup,        /**< see poll(3) */
    _ev_pollnval        /**< see poll(3) */
} _native_fd_event_t;

/**
 * Native internal file descriptor
 */
typedef int _native_fd_t;

/**
 * Add a file descriptor to natives list of open file descriptors.
 *
 * @param[in] fd        a file descriptor (as obtained by open)
 *
 * @return              an internal fd on success, -1 otherwise
 */
_native_fd_t _native_fd_add(int fd);

/**
 * Add a file descriptor to natives list of open file descriptors.
 *
 * @param[in] fd    a native internal file descriptor (as obtained by _native_fd_add)
 *
 * @return          0 on success, -1 otherwise
 */
int _native_fd_remove(_native_fd_t fd);

/**
 * Associate a handler with events on a file descriptor.
 *
 * @param[in] fd        a file descriptor that has been added with
 *                      _native_pfd_add_fd before
 * @param[in] event     an event to be monitored
 * @param[in] handler   callback for @ref event on @ref fd
 *
 * @return 0 on success, -1 otherwise
 */
int _native_fd_set_handler(_native_fd_t fd, _native_fd_event_t event, _native_callback_t handler);

/**
 * Close all active file descriptors.
 *
 * @return 0 on success, -1 otherwise
 */
int _native_fd_close_all(void);

/**
 * Initialize the native file descriptor infrastructure
 *
 * @note needs interrupts working
 */
void _native_fd_init(void);
