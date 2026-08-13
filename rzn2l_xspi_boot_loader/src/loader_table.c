#include "loader_table.h"

extern const app_image_manifest_t IMAGE_APP_FLASH_section_start;

static loader_table g_loader_table[TABLE_ENTRY_NUM];
static uintptr_t    g_application_entry;

bool loader_table_init(void)
{
  app_image_manifest_t const * const manifest = &IMAGE_APP_FLASH_section_start;

  if ((APP_IMAGE_MAGIC != manifest->magic) ||
    (APP_IMAGE_FORMAT_VERSION != manifest->format_version) ||
    (TABLE_ENTRY_NUM != manifest->entry_count))
  {
    return false;
  }

  for (uint32_t index = 0U; index < TABLE_ENTRY_NUM; index++)
  {
    g_loader_table[index].src         = (uintptr_t *) (uintptr_t) manifest->entries[index].source_address;
    g_loader_table[index].dst         = (uintptr_t *) (uintptr_t) manifest->entries[index].destination_address;
    g_loader_table[index].size        = (uintptr_t) manifest->entries[index].size;
    g_loader_table[index].enable_flag = manifest->entries[index].enable_flag;
  }

  g_application_entry = (uintptr_t) manifest->entry_point;

  return true;
}

const loader_table * loader_table_get(void)
{
  return g_loader_table;
}

uintptr_t loader_application_entry(void)
{
  return g_application_entry;
}
