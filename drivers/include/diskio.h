/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2010
/-----------------------------------------------------------------------*/

/**
 * @defgroup    diskio Disk IO Driver
 * @ingroup     drivers
 * @brief       Low level disk interface
 *
 * The connection between the MCU and the SRF08 is based on the i2c-interface.
 *
 * @{
 *
 * @file        diskio.h
 *
 * @author      unknown
 */

#ifndef DEF_DISKIO
#define DEF_DISKIO

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DN_MCI      0   /* Physical drive number for MCI */
#define DN_NAND     1   /* Physical drive number for NAND flash */

/**
 * @def     MCI_PWRSAVE
 * @ingroup conf
 * @brief   Powerdown mode to use between mci operations
 */
#ifndef MCI_PWRSAVE
#define MCI_PWRSAVE         0
#endif

/* These functions are defined in asmfunc.S */
void Copy_al2un(unsigned char *dst, const unsigned long *src, int count);   /* Copy aligned to unaligned. */
void Copy_un2al(unsigned long *dst, const unsigned char *src, int count);   /* Copy unaligned to aligned. */


/* Status of Disk Functions */
typedef unsigned char   DSTATUS;

/* Results of Disk Functions */
typedef enum {
    RES_OK = 0,     /* 0: Successful */
    RES_ERROR,      /* 1: R/W Error */
    RES_WRPRT,      /* 2: Write Protected */
    RES_NOTRDY,     /* 3: Not Ready */
    RES_PARERR      /* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize(unsigned char);
DSTATUS disk_status(unsigned char);
DRESULT disk_read(unsigned char, unsigned char *, unsigned long, unsigned char);
DRESULT disk_write(unsigned char, const unsigned char *, unsigned long, unsigned char);
DRESULT disk_ioctl(unsigned char, unsigned char, void *);



/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT      0x01    /* Drive not initialized */
#define STA_NODISK      0x02    /* No medium in the drive */
#define STA_PROTECT     0x04    /* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic ioctl command (defined for FatFs) */
#define CTRL_SYNC           0   /* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT    1   /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE     2   /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE      3   /* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR   4   /* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic ioctl command */
#define CTRL_POWER          5   /* Get/Set power status */
#define CTRL_LOCK           6   /* Lock/Unlock media removal */
#define CTRL_EJECT          7   /* Eject media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE        10  /* Get card type */
#define MMC_GET_CSD         11  /* Get CSD */
#define MMC_GET_CID         12  /* Get CID */
#define MMC_GET_OCR         13  /* Get OCR */
#define MMC_GET_SDSTAT      14  /* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV         20  /* Get F/W revision */
#define ATA_GET_MODEL       21  /* Get model name */
#define ATA_GET_SN          22  /* Get serial number */

/* NAND specific ioctl command */
#define NAND_FORMAT         30  /* Create physical format */



/*---------------------------------------------*/
/* Prototypes for each physical disk functions */


DSTATUS NAND_initialize(void);
DSTATUS NAND_status(void);
DRESULT NAND_read(unsigned char *, unsigned long, unsigned char);
DRESULT NAND_write(const unsigned char *, unsigned long, unsigned char);
DRESULT NAND_ioctl(unsigned char, void *);

DSTATUS MCI_initialize(void);
DSTATUS MCI_status(void);
DRESULT MCI_read(unsigned char *, unsigned long, unsigned char);
DRESULT MCI_write(const unsigned char *, unsigned long, unsigned char);
DRESULT MCI_ioctl(unsigned char, void *);
void MCI_timerproc(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
