/*
 * flash_map.c
 *
 *  Created on: Oct 17, 2024
 *      Author: mickey
 */

#include "flash_map.h"

const FlashSectorConfig_t sector_configs[FLASH_USED_SECTORS_COUNT] =
{
	{ FLASH_SECTOR_4_ADDRESS, FLASH_SECTOR_4_SIZE_BYTES, FLASH_SECTOR_4 },
	{ FLASH_SECTOR_5_ADDRESS, FLASH_SECTOR_5_SIZE_BYTES, FLASH_SECTOR_5 },
	{ FLASH_SECTOR_6_ADDRESS, FLASH_SECTOR_6_SIZE_BYTES, FLASH_SECTOR_6 },
	{ FLASH_SECTOR_7_ADDRESS, FLASH_SECTOR_7_SIZE_BYTES, FLASH_SECTOR_7 },
};

static void increment_tail_sector(FlashMap_t *map);

void flash_map_save(FlashMap_t *map, uint32_t dstAddress)
{
	uint8_t *castSrcPtr = (uint8_t *)map;
	flash_erase_sector(FLASH_SECTOR_3);
	flash_write(FLASH_SECTOR_3_ADDRESS, castSrcPtr, sizeof(*map));
}

void flash_map_load_nonalloc(FlashMap_t *map, uint32_t dstAddress)
{
	flash_read(dstAddress, (uint8_t *)map, sizeof(FlashMap_t));
}

void flash_map_initialize_nonalloc(FlashMap_t * map, uint8_t numSectors, const FlashSectorConfig_t *initData, bool eraseAll)
{
    // TODO: invalid input handling

	map->map_erased_flag = 0;
	map->map_version_number = FLASH_MAP_VERSION;
	map->sectors_count = numSectors;
    map->head_sector_index = 0;
    map->tail_sector_index = 0;

    for (uint8_t i = 0; i < numSectors; i++)
    {
        map->sectors_erased_flags[i] = 0;
        map->sectors_numbers[i] = initData[i].sector_number;
        map->sectors_lengths_bytes[i] = initData[i].sector_size_bytes;
        map->sectors_addresses[i] = initData[i].sector_address;
        map->sectors_write_offsets[i] = 0;

        if (eraseAll)
        {
			flash_erase_sector(map->sectors_numbers[i]);
        }
    }
}

uint8_t flash_map_append_string(FlashMap_t *map, uint8_t *new_string)
{
    uint8_t string_length = strlen((char *)new_string);
	uint32_t destination_address;

	// if trying to write to sector start, check if sector erased
	if (map->sectors_write_offsets[map->tail_sector_index] == 0)
	{
		// if new sector not erased, erase it ..
		if (0 == map->sectors_erased_flags[map->tail_sector_index])
		{
			flash_erase_sector(map->sectors_numbers[map->tail_sector_index]);
			map->sectors_erased_flags[map->tail_sector_index] = 1;
			map->sectors_write_offsets[map->tail_sector_index] = 0;
		}
	}

	// if current sector is maxed out, we need to target the next one
    if (map->sectors_write_offsets[map->tail_sector_index] + string_length + 1
    	>= map->sectors_lengths_bytes[map->tail_sector_index])
	{
		increment_tail_sector(map);
	}

	destination_address =
		map->sectors_addresses[map->tail_sector_index]
		+ map->sectors_write_offsets[map->tail_sector_index];

    // write the string length encoding byte !
    flash_write(destination_address, &string_length, 1);
    // write the actual string
	flash_write(destination_address + 1, new_string, string_length);

	// increment write target for next time
	map->sectors_write_offsets[map->tail_sector_index] += string_length + 1;

    // return string_length for debugging probably
    return string_length;
}

uint8_t flash_map_get_string_nonalloc(FlashMap_t *map, uint8_t sector_index, uint32_t sector_offset_address, uint8_t *return_buffer)
{
	uint32_t source_address = map->sectors_addresses[sector_index] + sector_offset_address;
    uint8_t string_length;
    // read the length byte into the string_length variable
    flash_read(source_address, &string_length, 1);
    // read the actual string using the length byte !
	flash_read(source_address+1, return_buffer, string_length);
    
    // return the length of the read string since we're gonna need it
    return string_length;
}

// static functions
static void increment_tail_sector(FlashMap_t *map)
{
	uint8_t new_tail = map->tail_sector_index + 1;

	// check bounds and cycle around if necessary
	if (new_tail >= map->sectors_count) new_tail = 0;

	// check if head needs to be erased and incremented
	if (new_tail == map->head_sector_index)
	{
		uint8_t new_head = new_tail + 1;

		// check bounds and cycle around if necessary
		// this also takes care of the case of just one sector
		if (new_head >= map->sectors_count) new_head = 0;

		// if necessary, erase the next sector to prepare for writing
		if (0 != map->sectors_erased_flags[new_tail])
		{
			flash_erase_sector(map->sectors_numbers[new_tail]);
			map->sectors_erased_flags[new_tail] = 1;
			map->sectors_write_offsets[new_tail] = 0;
		}

		// Apply new head!
		map->head_sector_index = new_head;
	}

	// Apply new tail!
	map->tail_sector_index = new_tail;
}
