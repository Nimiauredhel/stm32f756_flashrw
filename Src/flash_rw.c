/*
 * flash_rw.c
 *
 *  Created on: Oct 16, 2024
 *      Author: mickey
 */

#include "flash_rw.h"

typedef uint64_t FlashData_t;
#define FLASH_DATA_SIZE sizeof(FlashData_t)

void flash_erase_sector(uint32_t sector_to_erase)
{
	FLASH_EraseInitTypeDef flash_erase_struct = {0};
	uint32_t error_status = 0;

	HAL_FLASH_Unlock();

	// filling in the flash erase struct
	flash_erase_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
	flash_erase_struct.Sector = sector_to_erase;
	flash_erase_struct.NbSectors = 1;

	HAL_FLASHEx_Erase(&flash_erase_struct, &error_status);

	HAL_FLASH_Lock();
}

void flash_read(uint32_t srcAdr, uint8_t *dstPtr, uint16_t length)
{

	HAL_FLASH_Unlock();

	HAL_FLASH_Lock();
}

void flash_write(uint32_t dstAdr, uint8_t *srcPtr, uint16_t length)
{
	uint8_t double_word_data[FLASH_DATA_SIZE];

	HAL_FLASH_Unlock();

	HAL_FLASH_Lock();
}
