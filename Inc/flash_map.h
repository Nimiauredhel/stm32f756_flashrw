/*
 * flash_map.h
 *
 *  Created on: Oct 17, 2024
 *      Author: mickey
 */

#ifndef FLASH_MAP_H_
#define FLASH_MAP_H_

#include <stdint.h>

#define FLASH_USED_SECTORS_COUNT 4
#define FLASH_STRING_LENGTH 64

#define FLASH_SECTOR_4_ADDRESS 0x08020000
#define FLASH_SECTOR_5_ADDRESS 0x08040000
#define FLASH_SECTOR_6_ADDRESS 0x08080000
#define FLASH_SECTOR_7_ADDRESS 0x080C0000

#define FLASH_SECTOR_4_CAPACITY 2000
#define FLASH_SECTOR_5_CAPACITY 4000
#define FLASH_SECTOR_6_CAPACITY 4000
#define FLASH_SECTOR_7_CAPACITY 4000

typedef struct FlashMap
{
	// equals zero if map exists; else it must be initialized
	uint8_t map_erased;
	// version number to handle potential future structural changes
	uint16_t map_version;
	// number of sectors managed by this map struct
	uint8_t sectors_count;
	// length (in bytes) of each individual string
	uint8_t strings_length_bytes;
	// index (within struct) of the oldest-written sector
	uint8_t head_sector_index;
	// index (within struct) of the latest written sector
	uint8_t tail_sector_index;
	// address to which the next string should be written
	uint32_t next_write_address;
	// equals 0 if sector erased, 1 if sector written
	uint8_t sectors_used[FLASH_USED_SECTORS_COUNT];
	// total number of strings each sector can hold
	uint16_t sectors_string_capacity[FLASH_USED_SECTORS_COUNT];
	// memory address of each sector
	uint32_t sectors_addresses[FLASH_USED_SECTORS_COUNT];
} FlashMap_t;


#endif /* FLASH_MAP_H_ */
