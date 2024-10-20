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

FlashMap_t flash_map_initialize(uint8_t numSectors, const FlashSectorConfig_t *initData, bool eraseAll)
{
    // TODO: invalid input handling

	FlashMap_t new_map;

	new_map.map_erased_flag = 0;
	new_map.map_version_number = FLASH_MAP_VERSION;
	new_map.sectors_count = numSectors;
    new_map.string_length_bytes = FLASH_STRING_LENGTH_BYTES;
    new_map.head_sector_index = 0;
    new_map.tail_sector_index = 0;
    new_map.next_string_write_index = 0;

    for (uint8_t i = 0; i < numSectors; i++)
    {
        new_map.sectors_erased_flags[i] = 0;
        new_map.sectors_numbers[i] = initData[i].sector_number;
        new_map.sectors_string_capacities[i] = initData[i].sector_size_bytes / FLASH_STRING_LENGTH_BYTES;
        new_map.sectors_addresses[i] = initData[i].sector_address;

        if (eraseAll)
        {
			flash_erase_sector(new_map.sectors_numbers[i]);
        }
    }

    return new_map;
}

void flash_map_append_string(FlashMap_t *map, uint8_t *new_string)
{
	// if current sector is maxed out, we need to target the next one
	if(map->next_string_write_index >
    		map->sectors_string_capacities[map->tail_sector_index])
	{
		increment_tail_sector(map);
	}

	// if trying to write
	if (map->next_string_write_index == 0
			&& 0 == map->sectors_erased_flags[map->tail_sector_index])
	{
		flash_erase_sector(map->sectors_numbers[map->tail_sector_index]);
		map->sectors_erased_flags[map->tail_sector_index] = 1;
	}

	uint32_t destination_address =
			map->sectors_addresses[map->tail_sector_index]
		    + (map->next_string_write_index * map->string_length_bytes);

	flash_write(destination_address, new_string, map->string_length_bytes);

	// increment write target for next time
	map->next_string_write_index++;
}

void flash_map_get_string_nonalloc(FlashMap_t *map, uint8_t sector_index, uint8_t string_index, uint8_t *return_buffer)
{
	uint32_t source_address =
			(map->sectors_addresses[sector_index])
			+ (string_index * map->string_length_bytes);
	flash_read(source_address, return_buffer, map->string_length_bytes);
}

// static functions
static void increment_tail_sector(FlashMap_t *map)
{
	uint8_t new_tail = map->tail_sector_index + 1;

	// check bounds and cycle around if necessary
	if (new_tail >= map->sectors_count) new_tail = 0;

	// new sector, so set next index to zero
	map->next_string_write_index = 0;

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
		}
	}
}
