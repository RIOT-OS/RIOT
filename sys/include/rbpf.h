/*
 * Copyright (C) 2023 Freie Universität Berlin
 * Copyright (C) 2023 Inria
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_rbpf rBPF small virtual machine
 * @ingroup     sys
 * @brief       Small eBPF style virtual machine implementation for RIOT
 * @experimental
 *
 * ### Intro
 *
 * This implements a small virtual machine for use within RIOT called rBPF. As
 * the name indicates the virtual machine is based on eBPF and implements the
 * same ISA.
 *
 * The virtual machine implemented here is optimized for running short, limited
 * tasks which are guaranteed to exit. This makes it suitable to hook it into
 * RIOT internals.
 *
 * ### Setting up a virtual machine
 *
 * Setting up a virtual machine consists of a few steps:
 *
 *  1. Load the application on the RIOT instance
 *  2. Configure the rbpf application struct
 *  3. Launch the virtual machine with a context
 *
 * ```
 * static uint8_t _bpf_stack[RBPF_STACK_SIZE];
 * rbpf_application_t rbpf = { 0 };
 * rbpf_application_setup(&rbpf, _bpf_stack, (const rbpf_application_t*)&test_app, sizeof(test_app));
 * int64_t exec_result;
 * int result = rbpf_application_run_ctx(&rbpf, NULL, 0, &exec_result);
 * ```
 *
 * The result returned from @ref rbpf_application_run_ctx is the return code of
 * the virtual machine itself and indicates whether the application inside
 * executed correctly. The result argument of the function contains the value
 * returned by the application inside the virtual machine.
 *
 * ### Communicating with the virtual machine
 *
 * Passing information in and out of the virtual machine can be done via a
 * context struct supplied during the start of the execution. The definition of
 * this struct must match between the RIOT and the application inside. rBPF is a
 * 64 bit machine and as RIOT supports multiple platforms with different word
 * sizes it is strongly recommended to always use fixed-size members.
 *
 * One caveat is the size of a pointer, for which a macro @ref __bpf_shared_ptr
 * is provided. This ensures that pointers part of a struct are padded by having
 * them as part of a union with a 64 bit dummy value. For example:
 *
 * ```
 * typedef struct {
 *     __bpf_shared_ptr(uint32_t *, bar);
 *     uint32_t baz;
 * } foo;
 *```
 *
 * ### Memory protection
 *
 * The memory space of the virtual machine is shared with the host system, no
 * memory mapping is done between the two. To protect the host system against
 * the virtual machine, a rough software memory protection system is
 * implemented. This allows configuring different memory regions to the virtual
 * machine with either write or read (or both) permissions. Adding an extra
 * region can be done with:
 *
 * ```
 * uint8_t buf[BUF_LEN];
 *
 * rbpf_mem_region_t region;
 * rbpf_memory_region_init(
 *     &region,
 *     buf,
 *     BUF_LEN,
 *     RBPF_MEM_REGION_READ | RBPF_MEM_REGION_WRITE
 * );
 *
 * rbpf_add_region(&rbpf_application, &region);
 * ```
 *
 * The rbpf engine itself ensures correct memory permissions to the application
 * regions, stack and context struct.
 *
 * ### Application format
 *
 * The binary format of a full application consists of:
 *  - A header
 *  - The data section
 *  - The read-only data section
 *  - The application code itself
 *  - A list of functions exposed
 *
 *  The header contains all the necessary information to parse the application.
 *  It includes a magic number to verify the header. There is a version field to
 *  allow for basic compatibility checks. The flags are used as mechanism to
 *  communicate some of the settings to the virtual machine.
 *
 *  The rest of the header contains the length of all binary sections. The only
 *  restriction is that the application text section starts at a multiple of 8
 *  bytes offset from the start of the header. This allows the loader to align
 *  the text section. No assumption must be made on the alignment of the other
 *  sections
 *
 * @{
 *
 * @file
 * @brief Interface definitions for rBPF virtual machines
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef RBPF_H
#define RBPF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stack size inside the virtual machine, fixed size per specification
 */
#define RBPF_STACK_SIZE  (512)

/**
 * @brief Magic number for the header
 */
#define RBPF_MAGIC_NO (0x72425046)

/**
 * @brief Header for rBPF applications
 */
typedef struct __attribute__((packed)) {
    uint32_t magic;         /**< Magic number */
    uint32_t version;       /**< Version of the application */
    uint32_t flags;         /**< Flags for this application */
    uint32_t data_len;      /**< Length of the data section */
    uint32_t rodata_len;    /**< Length of the rodata section */
    uint32_t text_len;      /**< Length of the text section */
    uint32_t functions;     /**< Number of functions available */
} rbpf_header_t;

/**
 * @brief Data structure defining a function exposed by the application
 */
typedef struct __attribute__((packed)) {
    uint16_t name_offset;       /**< Offset in the rodata for the name */
    uint16_t flags;             /**< Flags for this function */
    uint16_t location_offset;   /**< Location in the text section where the function starts */
} rbpf_function_t;

/**
 * @brief rBPF Virtual Machine exit codes
 */
enum {
    RBPF_CONTINUE               = 1,    /**< Next instruction, never returned to user */
    RBPF_OK                     = 0,    /**< Successful execution */
    RBPF_ILLEGAL_INSTRUCTION    = -1,   /**< Failed on instruction parsing */
    RBPF_ILLEGAL_MEM            = -2,   /**< Illegal memory load */
    RBPF_ILLEGAL_JUMP           = -3,   /**< Jump offset not allowed */
    RBPF_ILLEGAL_CALL           = -4,   /**< Illegal call instruction, called function not known */
    RBPF_ILLEGAL_LEN            = -5,   /**< Invalid length of application */
    RBPF_ILLEGAL_REGISTER       = -6,   /**< Instruction register argument invalid */
    RBPF_NO_RETURN              = -7,   /**< No valid return found in the application code */
    RBPF_OUT_OF_BRANCHES        = -8,   /**< Number of branches taken is more than allowed */
    RBPF_ILLEGAL_DIV            = -9,   /**< Divide by zero error in instructions */
};

/**
 * @brief Forward declaration of the memory region
 */
typedef struct rbpf_mem_region rbpf_mem_region_t;

/**
 * @name Memory permission flags
 * @{
 */
#define RBPF_MEM_REGION_READ     0x01   /**< Memory region has reads allowed */
#define RBPF_MEM_REGION_WRITE    0x02   /**< Memory region has writes allowed */
/** @} */


/**
 * @brief rBPF memory region
 *
 * Defines the permissions on memory regions. Default permission is reject.
 */
struct rbpf_mem_region {
    rbpf_mem_region_t *next;    /**< Linked list ptr */
    const uint8_t *start;       /**< Start address */
    size_t len;                 /**< Length of the region in bytes */
    uint8_t flags;              /**< Permission flags */
};

/**
 * @name Internal rBPF struct flags
 * @{
 */
#define RBPF_FLAG_SETUP_DONE        0x01    /**< Initial setup of vm done */
#define RBPF_FLAG_PREFLIGHT_DONE    0x02    /**< Pre-flight checks executed at least once */
#define RBPF_CONFIG_NO_RETURN       0x0100  /**< Script doesn't need to have a return */
/** @} */

/**
 * @brief rBPF application
 */
typedef struct {
    rbpf_mem_region_t stack_region;     /**< Memory permission region for the stack */
    rbpf_mem_region_t rodata_region;    /**< Memory permissions for the application read-only data */
    rbpf_mem_region_t data_region;      /**< Memory permissions for the application data region */
    rbpf_mem_region_t arg_region;       /**< Memory region for the caller-supplied arguments */
    const void *application;            /**< Application header */
    size_t application_len;             /**< Application length */
    uint8_t *stack;                     /**< VM stack, must be  and aligned */
    uint16_t flags;                     /**< State flags for the virtual machine */
    uint32_t branches_remaining;        /**< Number of allowed branch instructions remaining */
} rbpf_application_t;

/**
 * @brief rBPF syscall interface function
 *
 * @param rbpf  rBPF application calling the function
 * @param regs  Register state of the virtual machine
 */
typedef uint32_t (*rbpf_call_t)(rbpf_application_t *rbpf, uint64_t *regs);

/**
 * @brief Initialize a new rBPF application
 *
 * @param rbpf              rBPF application to initialize
 * @param stack             Stack space to use for this application, must be 512 bytes
 * @param application       Application to load
 * @param application_len   Size of the whole application (including header) in bytes
 */
void rbpf_application_setup(rbpf_application_t *rbpf, uint8_t *stack,
                            const rbpf_application_t *application, size_t application_len);

/**
 * @brief Manually run the pre-flight checks for an application
 *
 * @param   rbpf    rBPF application to run the checks for
 *
 * @return  Negative on error
 */
int rbpf_application_verify_preflight(rbpf_application_t *rbpf);

/**
 * @brief Execute the rBPF virtual machine with a supplied context.
 *
 * Also runs the pre-flight checks if not yet done.
 *
 * @param   rBPF    rBPF application to launch
 * @param   ctx     Context struct to supply to the virtual machine
 * @param   ctx_size    Size of the context in bytes
 * @param   result  Result returned by the application inside the virtual machine
 *
 * @returns execution result of the virtual machine, negative on error
 */
int rbpf_application_run_ctx(rbpf_application_t *rbpf, void *ctx, size_t ctx_size, int64_t *result);

/**
 * @brief Initialize a memory region
 *
 * Intentionally doesn't touch the linked list pointer of the struct
 *
 * @param   region  Memory region to initialize
 * @param   start   Start address of the region
 * @param   len     Length of the region
 * @param   flags   Permission flags
 */
static inline void rbpf_memory_region_init(rbpf_mem_region_t *region, void *start, size_t len,
                                           uint8_t flags)
{
    region->start = start;
    region->len = len;
    region->flags = flags;
}

/**
 * @brief Add a memory region to the virtual machine
 *
 * @param   rbpf    The Application to add the memory region for
 * @param   region  The memory region to add
 */
void rbpf_add_region(rbpf_application_t *rbpf, rbpf_mem_region_t *region);

/**
 * @brief   Check if a store operation is allowed by the virtual machine with an address and size
 *
 * @param   rbpf    The rBPF application to check for
 * @param   addr    The start address of the write
 * @param   size    The size of the write
 *
 * @return True if allowed, false if not allowed
 */
bool rbpf_store_allowed(const rbpf_application_t *rbpf, void *addr, size_t size);

/**
 * @brief   Check if a load/read operation is allowed by the virtual machine with an address and size
 *
 * @param   rbpf    The rBPF application to check for
 * @param   addr    The start address of the read
 * @param   size    The size of the read
 *
 * @return True if allowed, false if not allowed
 */
bool rbpf_load_allowed(const rbpf_application_t *rbpf, void *addr, size_t size);

static inline const rbpf_header_t *rbpf_header(const rbpf_application_t *rbpf)
{
    return (rbpf_header_t *)rbpf->application;
}

/**
 * @brief Get a pointer to the rBPF application read-only data section
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The pointer of the rBPF applications read-only data
 */
static inline void *rbpf_application_rodata(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return (uint8_t *)header + sizeof(rbpf_application_t) + header->data_len;
}

/**
 * @brief Get the length of the rBPF application read-only data section
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The length in bytes of the rBPF applications read-only data section
 */
static inline size_t rbpf_application_rodata_len(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return header->rodata_len;
}

/**
 * @brief Get a pointer to the rBPF application data section
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The pointer of the rBPF applications data
 */
static inline void *rbpf_application_data(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return (uint8_t *)header + sizeof(rbpf_application_t);
}

/**
 * @brief Get the length of the rBPF application data section
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The length in bytes of the rBPF applications data section
 */
static inline size_t rbpf_application_data_len(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return header->data_len;
}

/**
 * @brief Get a pointer to the rBPF application text
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The pointer of the rBPF applications text
 */
static inline const void *rbpf_application_text(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return (uint8_t *)header + sizeof(rbpf_header_t) + header->data_len + header->rodata_len;
}

/**
 * @brief Get the length of the rBPF application text
 *
 * @param   rBPF    The rBPF application
 *
 * @return  The length in bytes of the rBPF applications text
 */
static inline size_t rbpf_application_text_len(const rbpf_application_t *rbpf)
{
    const rbpf_header_t *header = rbpf_header(rbpf);

    return header->text_len;
}

/* to be implemented by platform specifc code. */
void rbpf_store_init(void);

#ifdef __cplusplus
}
#endif
#endif /* RBPF_H*/

/** @} */
