/*
 * flash_map.c
 *
 *  Created on: Oct 17, 2024
 *      Author: mickey
 */

#include "flash_map.h"

static FlashMap_t inmemory_map;

static const FlashSectorConfig_t sector_configs[FLASH_USED_SECTORS_COUNT] =
{
	{ FLASH_SECTOR_4_ADDRESS, FLASH_SECTOR_4_SIZE_BYTES },
	{ FLASH_SECTOR_5_ADDRESS, FLASH_SECTOR_5_SIZE_BYTES },
	{ FLASH_SECTOR_6_ADDRESS, FLASH_SECTOR_6_SIZE_BYTES },
	{ FLASH_SECTOR_7_ADDRESS, FLASH_SECTOR_7_SIZE_BYTES },
};

void flash_map_save(FlashMap_t *map, uint32_t dstAddress)
{
	uint32_t *serialized_map = malloc(sizeof(FlashMap_t));
	*serialized_map = *(uint32_t *)map;
}

void flash_map_load(FlashMap_t *map, uint32_t dstAddress)
{
}

bool flash_map_initialize(FlashSectorConfig_t initData[FLASH_USED_SECTORS_COUNT])
{
    // TODO: invalid input handling

    FlashMap_t temp_map;
    flash_read(FLASH_SECTOR_4_ADDRESS, &temp_map, sizeof(FlashMap_t)); 
	FlashMap_t *new_map = malloc(sizeof(FlashMap_t));

	new_map->map_erased_flag = 0;
	new_map->map_version_number = FLASH_MAP_VERSION;
	new_map->sectors_count = FLASH_USED_SECTORS_COUNT;
    new_map->string_length_bytes = FLASH_STRING_LENGTH_BYTES;
    new_map->head_sector_index = 0;
    new_map->tail_sector_index = 0;
    new_map->next_string_write_index = initData[0].sector_address;

    for (uint8_t i = 0; i < FLASH_USED_SECTORS_COUNT; i++)
    {
        new_map->sectors_erased_flags[i] = 0;
        new_map->sectors_string_capacities[i] = initData[i].sector_size_bytes / FLASH_STRING_LENGTH_BYTES;
        new_map->sectors_addresses[i] = initData[i].sector_address;
    }

    return new_map;
}

void flash_map_append_string(FlashMap_t *map, uint8_t *new_string)
{
}

void flash_map_get_string_nonalloc(uint8_t sector_index, uint8_t string_index, uint8_t (*return_buffer)[FLASH_STRING_LENGTH_BYTES])
{
}
