/*
 * flashrom.c
 *
 *  Created on: 30.09.2016
 *      Author: geith
 */

#include "cc2538_flashrom.h"
#include "mutex.h"
#include "irq.h"
#include "stdio.h"

cc2538_flash_fctl_t* const FCTL = (cc2538_flash_fctl_t*)&FLASH_CTRL_FCTL;
cc2538_flash_faddr_t* const FADDR = (cc2538_flash_faddr_t*)&FLASH_CTRL_FADDR;

static mutex_t flash_mutex = MUTEX_INIT;

#define FLASHROM_SECTION_DATA_INIT 1

#if FLASHROM_SECTION_DATA_INIT
/*
 * don't know why, but writing flash works only if the address of
 * _flashrom_do_write has been read once (function is places in
 * .data section in RAM instead of flash)
 */
static volatile char init_done = 0;
void* _flashrom_write_addr;
#endif


static uint
flashrom_addr_to_page(uint8_t* addr){
	uint page_num = 0;
	uint flash_page_size = FLASH_SIZE / FLASH_PAGE_NUM;

	uint8_t* rel_addr = addr - FLASH_BASE;

	page_num = (uint)rel_addr / (uint)flash_page_size;

	return page_num;
}

uint8_t
flashrom_erase(uint8_t *addr)
{
	mutex_lock(&flash_mutex);
	irq_disable();

	while(FCTL->FCTLbits.BUSY);
	FLASH_CTRL_FADDR = (cc2538_reg_t) addr;
	FCTL->FCTLbits.ERASE = 1;
	while(FCTL->FCTLbits.BUSY);

	irq_enable();
	mutex_unlock(&flash_mutex);

	return 1;
}

static
__attribute__((section(".data")))
void
_flashrom_do_write(uint8_t *dst, const uint8_t *src, size_t size){

	while(FCTL->FCTLbits.BUSY);


	uint32_t tmp_data;
	uint32_t *tmp_data_ptr;

	FLASH_CTRL_FADDR = (cc2538_reg_t) dst;
	FCTL->FCTLbits.WRITE = 1;

	for(size_t s = 0; s < size; s+=sizeof(tmp_data)){
		tmp_data_ptr = (uint32_t*) (src + s);
		tmp_data = *tmp_data_ptr;

		FLASH_CTRL_FWDATA = tmp_data;
		while(FCTL->FCTLbits.FULL);
	}
	FCTL->FCTLbits.WRITE = 0;
	return;
}

uint8_t flashrom_write(uint8_t *dst, const uint8_t *src, size_t size)
{

#if FLASHROM_SECTION_DATA_INIT
	if(!init_done){
		_flashrom_write_addr = &_flashrom_do_write;
		init_done = 1;
	}
#endif

	uint page_num = flashrom_addr_to_page(dst);

	if(page_num == 0xff){
		return 0;
	}

	mutex_lock(&flash_mutex);
	irq_disable();

	_flashrom_do_write(dst, src, size);

	irq_enable();
	mutex_unlock(&flash_mutex);

	return 1;
}
