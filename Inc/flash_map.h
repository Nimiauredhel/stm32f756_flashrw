/*
 * flash_map.h
 *
 *  Created on: Oct 17, 2024
 *      Author: mickey
 */

#ifndef FLASH_MAP_H_
#define FLASH_MAP_H_

#include "flash_rw.h"

#define FLASH_MAP_VERSION 4

#define FLASH_USED_SECTORS_COUNT 4
#define FLASH_STRING_LENGTH_BYTES 64

// sector 3 will hold the usage map for the other sectors
#define FLASH_SECTOR_3_ADDRESS 0x08018000 // decimal: 134,316,032
// sectors 4-7 will hold the actual data.
// the data will be written to a sector sequentially,
// but the sectors will be used cyclically like a ring buffer
#define FLASH_SECTOR_4_ADDRESS 0x08020000 // decimal: 134,348,800
#define FLASH_SECTOR_5_ADDRESS 0x08040000 // decimal: 134,479,872
#define FLASH_SECTOR_6_ADDRESS 0x08080000 // decimal: 134,742,016
#define FLASH_SECTOR_7_ADDRESS 0x080C0000 // decimal: 135,004,160

#define FLASH_SECTOR_4_SIZE_BYTES 131072
#define FLASH_SECTOR_5_SIZE_BYTES 262144
#define FLASH_SECTOR_6_SIZE_BYTES 262144
#define FLASH_SECTOR_7_SIZE_BYTES 262144

// struct that maps the usage of flash storage sectors,
// and is stored in a dedicated sector for persistence between power cycles.
// I'm "packing" it (but really just explicitly padding it) to 4 byte alignment
// to make sure that future revisions can be read and updated reliably,
// even if I end up not using 32 bit values
#pragma pack (4)
typedef struct FlashMap
{
	// equals zero if map exists; else it must be initialized
	uint8_t map_erased_flag;
	// version number to handle potential future structural changes
	uint16_t map_version_number;
	// number of sectors managed by this map struct
	uint8_t sectors_count;
	// index (within struct) of the oldest-written sector
	uint8_t head_sector_index;
	// index (within struct) of the latest written sector
	uint8_t tail_sector_index;
	// equals 1 if sector erased, 0 if sector written
	uint8_t sectors_erased_flags[FLASH_USED_SECTORS_COUNT];
	// total number of strings each sector can hold
	uint32_t sectors_lengths_bytes[FLASH_USED_SECTORS_COUNT];
	// hardware reference number of each sector
	uint32_t sectors_numbers[FLASH_USED_SECTORS_COUNT];
	// memory address of each sector
	uint32_t sectors_addresses[FLASH_USED_SECTORS_COUNT];
	// memory address of each sector
	uint32_t sectors_write_offsets[FLASH_USED_SECTORS_COUNT];
} FlashMap_t;

typedef struct FlashSectorConfig
{
	uint32_t sector_address;
	uint32_t sector_size_bytes;
	uint32_t sector_number;
} FlashSectorConfig_t;

extern const FlashSectorConfig_t sector_configs[FLASH_USED_SECTORS_COUNT];

void flash_map_save(FlashMap_t *map, uint32_t dstAddress);
void flash_map_load_nonalloc(FlashMap_t *map, uint32_t dstAddress);
void flash_map_initialize_nonalloc(FlashMap_t *map, uint8_t numSectors, const FlashSectorConfig_t *initData, bool eraseAll);
uint8_t flash_map_append_string(FlashMap_t *map, uint8_t *new_string);
uint8_t flash_map_get_string_nonalloc(FlashMap_t *map, uint8_t sector_index, uint32_t sector_offset_address, uint8_t *return_buffer);
uint32_t flash_map_sector_bytes_left(FlashMap_t *map);

#endif /* FLASH_MAP_H_ */
