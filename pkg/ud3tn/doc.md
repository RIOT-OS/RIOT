/**
 * @defgroup pkg_ud3tn uD3TN
 * @ingroup pkg
 * @brief Micro Delay-Tolerant Networking implementation for RIOT
 *
 * This package provides the RIOT platform port of uD3TN. It supports
 * Bundle Protocol version 6 and version 7 and uses RIOT's socket and
 * timer APIs.
 *
 * Add `USEPKG += ud3tn` to an application's Makefile to use the package.
 *
 * RIOT-specific sources are located in `pkg/ud3tn`, and patches in
 * `pkg/ud3tn/patches` adapt the pinned upstream revision.
 *
 * The POSIX AAP1 agent is not available in this port. The RIOT AAP2 agent
 * listens on IPv6 TCP port 4242 on all interfaces; restrict network access
 * where necessary.
 */
