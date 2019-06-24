#include <assert.h>
#include "cpu.h"
#include "periph/flashpage.h"
#include "vendor/rom.h"
#include "vendor/hw_flash_ctrl.h"
#include "log.h"

#define FLASH_ERASE_SIZE  0x800

uint32_t
FlashCacheModeGet(void)
{
    //
    // Return a FLASH_CACHE_MODE_<> macro value.
    //
    return(HWREG(FLASH_CTRL_FCTL) & FLASH_CTRL_FCTL_CM_M);
}
void
FlashCacheModeSet(uint32_t ui32CacheMode)
{
    uint32_t ui32Busy;
    uint32_t ui32TempValue;

    //
    // Check the arguments.
    //
    assert((ui32CacheMode == FLASH_CTRL_CACHE_MODE_DISABLE) ||
           (ui32CacheMode == FLASH_CTRL_CACHE_MODE_ENABLE) ||
           (ui32CacheMode == FLASH_CTRL_CACHE_MODE_PREFETCH_ENABLE) ||
           (ui32CacheMode == FLASH_CTRL_CACHE_MODE_REALTIME));
    //
    // Wait until FLASH is not busy.
    //
    ui32Busy = 1;
    while(ui32Busy)
    {
        ui32TempValue = HWREG(FLASH_CTRL_FCTL);
        ui32Busy      = ui32TempValue & FLASH_CTRL_FCTL_BUSY;
    }

    //
    // Set desired cache mode.
    //
    ui32TempValue           &= ~FLASH_CTRL_FCTL_CM_M;
    HWREG(FLASH_CTRL_FCTL) = ui32TempValue | ui32CacheMode;
}
//*****************************************************************************
//
//! Erases a flash main page with use of ROM function
//!
//! \param ui32Address is the start address of the flash main page to be erased.
//!
//! This function erases one 2 kB main page of the on-chip flash. After
//! erasing, the page is filled with 0xFF bytes. Locked pages cannot be
//! erased. The flash main pages do not include the upper page.
//!
//! This function does not return until the page is erased or an error
//! encountered.
//!
//! \return Returns 0 on success, -1 if erasing error is encountered,
//!         or -2 in case of illegal parameter use.
//
//*****************************************************************************
int32_t
FlashMainPageErase(uint32_t ui32Address)
{
    int32_t          i32Stat;               // 0 = pass, -1 = fail
    uint32_t ui32CurrentCacheMode;

    i32Stat = 0;
    ui32CurrentCacheMode = FlashCacheModeGet();

    //
    // Erase the specified flash main page by calling ROM function.
    //
    i32Stat = ROM_PageErase(ui32Address, FLASH_ERASE_SIZE);

    //
    // Restore cache mode.
    //
    FlashCacheModeSet(ui32CurrentCacheMode);

    //
    // Return status pass or fail.
    //
    return(i32Stat);
}

void flashpage_write(int page, const void *data)
{
    uint32_t page_addr;
    page_addr = (CPU_FLASH_BASE + (page * FLASHPAGE_SIZE));
    
    if (FlashMainPageErase(page_addr) != FLASHPAGE_OK) 
    {
		LOG_WARNING("riotboot_flashwrite: error erasing flashpage %d!\n",page);
	}
	
	uint32_t ui32CurrentCacheMode;
    int32_t  i32Stat;                // 0 = pass, -1 = fail, -2 = wrong param
    i32Stat = 0;                     // Start out passing

    //
    // Check the arguments.
    //
    assert(!(page_addr < (FLASH_BASE + (512 * 1024) -
                            FLASHPAGE_SIZE)));
    assert(!(page_addr & 3));
    ui32CurrentCacheMode = FlashCacheModeGet();
	i32Stat = ROM_ProgramFlash(data, page_addr, FLASHPAGE_SIZE);
	if (i32Stat == -1) 
    {
		LOG_WARNING("riotboot_flashwrite: programming error!\n");
	}
	if (i32Stat == -2) 
    {
		LOG_WARNING("riotboot_flashwrite: illegal parameter use!\n");
	}
	//
    // Restore cache mode.
    //
    FlashCacheModeSet(ui32CurrentCacheMode); 
}
