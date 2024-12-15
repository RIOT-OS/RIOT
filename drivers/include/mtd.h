/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd Memory Technology Device
 * @ingroup     drivers_storage
 * @{
 * @brief       Low level Memory Technology Device interface
 *
 * Generic memory technology device interface
 *
 * Unlike the @ref drivers_periph_flashpage, this is device driver based (i.e.
 * all functions take a @ref mtd_dev_t as a first argument), so that SPI based
 * EEPROMs (e.g. @ref drivers_mtd_at25xxx "AT25xxx") can be accessed the same
 * way as @ref drivers_mtd_flashpage "internal flash" or @ref
 * drivers_mtd_sdcard "SD cards"), all inside the same application.
 *
 * MTD devices expose a block based erase and write interface. In that, they
 * are the distinct from block devices (like hard disks) on which individual
 * bytes can be overwritten. The [Linux MTD FAQ](http://www.linux-mtd.infradead.org/faq/general.html)
 * has a convenient comparison (beware though of terminology differences
 * outlined below). They can be erased (with some granularity, often wearing
 * out the erased area a bit), and erased areas can be written to (sometimes
 * multiple times).
 *
 * MTD devices are described in terms of sectors, pages and feature flags:
 *
 * * A **sector** is the device's erase unit. Calls to @ref mtd_erase need to
 *   work in alignment with this number (commonly somewhere around 1kiB).
 *
 *   (Note that this corresponds to the term "page" as used in the flashpage
 *   API, and the term "eraseblock" in Linux's MTD).
 *
 * * A **page** is the largest a device can write in one transfer.
 *
 *   Applications rarely need to deal with this; it offers no guarantees on
 *   atomicity, but writing within a page is generally faster than across page
 *   boundaries.
 *
 *   Pages are a subdivision of sectors.
 *
 * * The **write size** is the minimum size of writes to the device, and also
 *   the required alignment of writes.
 *
 *   The write size is a divider of the page. It is often between 1 to 4 bytes
 *   long, but may be up to the full page size.
 *
 * * The device's **flags** indicate features, eg. whether a memory location
 *   can be overwritten without erasing it first.
 *
 * Unless a flag (such as @ref MTD_DRIVER_FLAG_DIRECT_WRITE or @ref
 * MTD_DRIVER_FLAG_CLEARING_OVERWRITE) allows it, this MTD API does not allow
 * memory areas to be written to twice between erase operations. Drivers are
 * not expected to count write accesses, and neither do this module's
 * functions: The performance impact would be too great. It is up to the
 * application to only write to erased memory once. Failure to do so may damage
 * hardware.
 *
 * This MTD API currently does not specify which value will be read from an
 * erased sector.
 *
 * @file
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef MTD_H
#define MTD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   MTD power states
 */
enum mtd_power_state {
    MTD_POWER_UP,    /**< Power up */
    MTD_POWER_DOWN,  /**< Power down */
};

/**
 * @brief   MTD driver interface
 *
 * This define the functions to access a MTD.
 *
 * A MTD is composed of pages combined into sectors. A sector is the smallest erasable unit.
 * The number of pages in a sector must be constant for the whole MTD.
 *
 * The erase operation is available only for entire sectors.
 */
typedef struct mtd_desc mtd_desc_t;

/**
 * @brief   MTD device descriptor
 *
 * See the @ref drivers_mtd "module level documentation" for details on the
 * field semantics.
 */
typedef struct {
    const mtd_desc_t *driver;  /**< MTD driver */
    uint32_t sector_count;     /**< Number of sector in the MTD */
    uint32_t pages_per_sector; /**< Number of pages per sector in the MTD */
    uint32_t page_size;        /**< Size of the pages in the MTD */
    uint32_t write_size;       /**< Minimum size and alignment of writes to the device */
#if defined(MODULE_MTD_WRITE_PAGE) || DOXYGEN
    void *work_area;           /**< sector-sized buffer (only present when @ref mtd_write_page is enabled) */
#endif
} mtd_dev_t;

/**
 * @brief   MTD device array as XFA
 *
 * The array contains the addresses of all MTD devices that are defined using
 * the @ref MTD_XFA_ADD macro, for example:
 * ```
 * MTD_XFA_ADD(my_dev, 0);
 * ```
 * The MTD devices in this array can be used for automatic functions such as
 * with the `mtd_default` module. The i-th device in this array can then be
 * accessed with `mtd_dev_xfa[i]`. The number of MTDs defined in this array
 * is `XFA_LEN(mtd_dev_xfa)`.
 */
#if !DOXYGEN
XFA_USE_CONST(mtd_dev_t *, mtd_dev_xfa);
#else
mtd_dev_t * const mtd_dev_xfa[];
#endif

/**
 * @brief   Define MTD device pointer variable `mtd<idx>`
 *
 * The macro defines the MTD device pointer variable `mtd<idx>`, sets it to
 * the address of the MTD device specified by the @p dev parameter, and adds
 * it to the XFA of MTD device pointers @ref mtd_dev_xfa. For example
 * ```
 * MTD_XFA_ADD(my_dev, 1);
 * ```
 * defines the variable `mtd1` pointing to the device `my_dev`.
 *
 * The parameter @p idx is used as priority of the MTD device pointer within
 * the XFA. That means it determines the order of the MTD device pointers
 * within @ref mtd_dev_xfa.
 *
 * @note Only if each MTD device is added with a unique priority and only if the
 *       priorities start at 0 and are used in consecutive order, the parameter
 *       @p idx corresponds to the position of the MTD device pointer within
 *       the @ref mtd_dev_xfa XFA and `mtd_dev_xfa[i]` points to the i-th MTD
 *       device.
 *
 * @param   dev     MTD device
 * @param   idx     Priority of the MTD device pointer within the XFA
 */
#define MTD_XFA_ADD(dev, idx) \
    XFA_CONST(mtd_dev_t, mtd_dev_xfa, idx) *mtd ## idx = (mtd_dev_t *)&(dev)

/**
 * @brief   Number of MTDs defined in the MTD device array in XFA
 */
#define MTD_NUMOF   XFA_LEN(mtd_dev_t *, mtd_dev_xfa)

/**
 * @brief   MTD driver can write any data to the storage without erasing it first.
 *
 * If this is set, a write completely overrides the previous values.
 */
#define MTD_DRIVER_FLAG_DIRECT_WRITE    (1 << 0)

/**
 * @brief   MTD driver supports arbitrary clearing overwrites
 *
 * If this is set, (arbitrarily) many writes are permitted per write size, and
 * the result is the old value bitwise-AND the written value.
 *
 * This property is common for managed flash memories. (By comparison, the raw
 * flash often used internally by MCUs may not allow overwrites, or may allow
 * them with the same semantics, but only for a limited number of writes
 * between erasures; there is currently no flag describing these any further).
 */
#define MTD_DRIVER_FLAG_CLEARING_OVERWRITE    (1 << 1)

/**
 * @brief   MTD driver interface
 *
 * This define the functions to access a MTD.
 *
 * A MTD is composed of pages combined into sectors. A sector is the smallest erasable unit.
 * The number of pages in a sector must be constant for the whole MTD.
 *
 * The erase operation is available only for entire sectors.
 */
struct mtd_desc {
    /**
     * @brief   Initialize Memory Technology Device (MTD)
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @retval 0 on success
     * @retval <0 value on error
     */
    int (*init)(mtd_dev_t *dev);

    /**
     * @brief   Read from the Memory Technology Device (MTD)
     *
     * No alignment is required on @p addr and @p size.
     *
     * @param[in]  dev      Pointer to the selected driver
     * @param[out] buff     Pointer to the data buffer to store read data
     * @param[in]  addr     Starting address
     * @param[in]  size     Number of bytes
     *
     * @retval 0 on success
     * @retval <0 value on error
     */
    int (*read)(mtd_dev_t *dev,
                void *buff,
                uint32_t addr,
                uint32_t size);

    /**
     * @brief   Read from the Memory Technology Device (MTD) using
     *          pagewise addressing.
     *
     * @p offset should not exceed the page size
     *
     * @param[in]  dev      Pointer to the selected driver
     * @param[out] buff     Pointer to the data buffer to store read data
     * @param[in]  page     Page number to start reading from
     * @param[in]  offset   Byte offset from the start of the page
     * @param[in]  size     Number of bytes
     *
     * @retval n number of bytes read on success
     * @retval <0 value on error
     */
    int (*read_page)(mtd_dev_t *dev,
                     void *buff,
                     uint32_t page,
                     uint32_t offset,
                     uint32_t size);

    /**
     * @brief   Write to the Memory Technology Device (MTD) using
     *          pagewise addressing.
     *
     * @p offset should not exceed the page size
     *
     * @param[in]  dev      Pointer to the selected driver
     * @param[out] buff     Pointer to the data to be written
     * @param[in]  page     Page number to start writing to
     * @param[in]  offset   Byte offset from the start of the page
     * @param[in]  size     Number of bytes
     *
     * @retval n bytes written on success
     * @retval <0 value on error
     */
    int (*write_page)(mtd_dev_t *dev,
                      const void *buff,
                      uint32_t page,
                      uint32_t offset,
                      uint32_t size);

    /**
     * @brief   Erase sector(s) over the Memory Technology Device (MTD)
     *
     * @p addr must be aligned on a sector boundary. @p size must be a multiple of a sector size.
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @retval 0 on success
     * @retval <0 value on error
     */
    int (*erase)(mtd_dev_t *dev,
                 uint32_t addr,
                 uint32_t size);

    /**
     * @brief   Erase sector(s) of the Memory Technology Device (MTD)
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] sector    the first sector number to erase

     * @param[in] count     Number of sectors to erase
     *
     * @retval 0 on success
     * @retval <0 value on error
     */
    int (*erase_sector)(mtd_dev_t *dev,
                        uint32_t sector,
                        uint32_t count);

    /**
     * @brief   Control power of Memory Technology Device (MTD)
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] power     Power state to apply (from @ref mtd_power_state)
     *
     * @retval 0 on success
     * @retval <0 value on error
     */
    int (*power)(mtd_dev_t *dev, enum mtd_power_state power);

    /**
     * @brief   Properties of the MTD driver
     */
    uint8_t flags;
};

/**
 * @brief   mtd_init Initialize a MTD device
 *
 * @param mtd the device to initialize
 *
 * @retval 0 on success
 * @retval <0 on error probably errno
 * @retval -ENODEV if no device if given or no driver is set
 * @retval -ENOTSUP if device has no init function
 */
int mtd_init(mtd_dev_t *mtd);

/**
 * @brief   Read data from a MTD device
 *
 * No alignment is required on @p addr and @p count.
 *
 * @param      mtd   the device to read from
 * @param[out] dest  the buffer to fill in
 * @param[in]  addr  the start address to read from
 * @param[in]  count the number of bytes to read
 *
 * @retval 0 on success
 * @retval <0 if an error occurred
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory
 * @retval -EIO if I/O error occurred
 */
int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count);

/**
 * @brief   Read data from a MTD device with pagewise addressing
 *
 * The MTD layer will take care of splitting up the transaction into multiple
 * reads if it is required by the underlying storage media.
 *
 * @param      mtd      the device to read from
 * @param[out] dest     the buffer to fill in
 * @param[in]  page     Page number to start reading from
 * @param[in]  offset   offset from the start of the page (in bytes)
 * @param[in]  size     the number of bytes to read
 *
 * @retval 0 on success
 * @retval <0 value on error
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory
 * @retval -EIO if I/O error occurred
 */
int mtd_read_page(mtd_dev_t *mtd, void *dest, uint32_t page, uint32_t offset, uint32_t size);

/**
 * @brief   Write data to a MTD device
 *
 * @p addr + @p count must be inside a page boundary. @p addr can be anywhere
 * but the buffer cannot overlap two pages.
 *
 * Both parameters must be multiples of the device's write size.
 *
 * @param      mtd   the device to write to
 * @param[in]  src   the buffer to write
 * @param[in]  addr  the start address to write to
 * @param[in]  count the number of bytes to write
 *
 * @retval 0 on success
 * @retval <0 if an error occurred
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory,
 *                    or overlapping two pages
 * @retval -EIO if I/O error occurred
 * @retval -EINVAL if parameters are invalid (invalid alignment for instance)
 */
int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count);

/**
 * @brief   Write data to a MTD device with pagewise addressing
 *
 * The MTD layer will take care of splitting up the transaction into multiple
 * writes if it is required by the underlying storage media.
 *
 * This performs a raw write, no automatic read-modify-write cycle is performed.
 *
 * Both @p offset and @p size must be multiples of the device's write size.
 *
 * @param      mtd      the device to write to
 * @param[in]  src      the buffer to write
 * @param[in]  page     Page number to start writing to
 * @param[in]  offset   byte offset from the start of the page
 * @param[in]  size     the number of bytes to write
 *
 * @retval 0 on success
 * @retval <0 value on error
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory,
 * @retval -EIO if I/O error occurred
 * @retval -EINVAL if parameters are invalid
 */
int mtd_write_page_raw(mtd_dev_t *mtd, const void *src, uint32_t page,
                       uint32_t offset, uint32_t size);

/**
 * @brief   Write data to a MTD device with pagewise addressing
 *
 * The MTD layer will take care of splitting up the transaction into multiple
 * writes if it is required by the underlying storage media.
 *
 * If the underlying sector needs to be erased before it can be written, the MTD
 * layer will take care of the read-modify-write operation.
 *
 * @p offset must be smaller than the page size
 *
 * @note this requires the `mtd_write_page` module
 *
 * @param      mtd      the device to write to
 * @param[in]  src      the buffer to write
 * @param[in]  page     Page number to start writing to
 * @param[in]  offset   byte offset from the start of the page
 * @param[in]  size     the number of bytes to write
 *
 * @retval 0 on success
 * @retval <0 value on error
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory,
 * @retval -EIO if I/O error occurred
 * @retval -EINVAL if parameters are invalid
 */
int mtd_write_page(mtd_dev_t *mtd, const void *src, uint32_t page,
                   uint32_t offset, uint32_t size);

/**
 * @brief   Erase sectors of a MTD device
 *
 * @p addr must be aligned on a sector boundary. @p count must be a multiple of a sector size.
 *
 * @param      mtd   the device to erase
 * @param[in]  addr  the address of the first sector to erase
 * @param[in]  count the number of bytes to erase
 *
 * @retval 0 if erase successful
 * @retval <0 if an error occurred
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory
 * @retval -EIO if I/O error occurred
 */
int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count);

/**
 * @brief   Erase sectors of a MTD device
 *
 * @param      mtd    the device to erase
 * @param[in]  sector the first sector number to erase
 * @param[in]  num    the number of sectors to erase
 *
 * @retval 0 if erase successful
 * @retval <0 if an error occurred
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p sector are not valid, i.e. outside memory
 * @retval -EIO if I/O error occurred
 */
int mtd_erase_sector(mtd_dev_t *mtd, uint32_t sector, uint32_t num);

/**
 * @brief   Write data to a MTD device with whole sector writes
 *
 * The MTD layer will take care of splitting up the transaction into multiple
 * writes if it is required by the underlying storage media.
 *
 * The sectors will be erased before writing if needed.
 *
 * @param      mtd      Device to write to
 * @param[in]  src      Buffer to write
 * @param[in]  sector   Sector number to start writing to
 * @param[in]  num      Number of sectors to write
 *
 * @retval 0 on success
 * @retval <0 value on error
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation is not supported on @p mtd
 * @retval -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory,
 * @retval -EIO if I/O error occurred
 * @retval -EINVAL if parameters are invalid
 */
int mtd_write_sector(mtd_dev_t *mtd, const void *src, uint32_t sector, uint32_t num);

/**
 * @brief   Set power mode on a MTD device
 *
 * @param      mtd   the device to access
 * @param[in]  power the power mode to set
 *
 * @retval 0 if power mode successfully set
 * @retval <0 if an error occurred
 * @retval -ENODEV if @p mtd is not a valid device
 * @retval -ENOTSUP if operation or @p power state is not supported on @p mtd
 * @retval -EIO if I/O error occurred
 */
int mtd_power(mtd_dev_t *mtd, enum mtd_power_state power);

/**
 * @brief   Get an MTD device by index
 *
 * @param[in] idx   Index of the MTD device
 *
 * @return  MTD_0 for @p idx 0 and so on
 *          NULL if no MTD device exists for the given index
 */
static inline mtd_dev_t *mtd_dev_get(unsigned idx)
{
    assert(MTD_NUMOF != 0);
    assert(idx < MTD_NUMOF);

    return mtd_dev_xfa[idx];
}

#ifdef __cplusplus
}
#endif

#endif /* MTD_H */
/** @} */
