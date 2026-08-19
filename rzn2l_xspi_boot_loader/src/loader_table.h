#include "bsp_api.h"

/* The image manifest is emitted by every split App linker script. */
#define APP_IMAGE_MAGIC                  (0x41505049UL) /* "APPI" */
#define APP_IMAGE_FORMAT_VERSION         (3UL)
#define TABLE_ENTRY_NUM                  (8U)
#define TABLE_ENABLE                     (1U)
#define TABLE_CLEAR                      (2U)
#define TABLE_DISABLE                    (0U)

typedef struct st_app_image_entry
{
    uint32_t source_address;
    uint32_t destination_address;
    uint32_t size;
    uint32_t enable_flag;
} app_image_entry_t;

typedef struct st_app_image_manifest
{
    uint32_t          magic;
    uint32_t          format_version;
    uint32_t          entry_count;
    uint32_t          entry_point;
    app_image_entry_t entries[TABLE_ENTRY_NUM];
} app_image_manifest_t;

/* loader_table definition */
typedef struct {
    uintptr_t * src;
    uintptr_t * dst;
    uintptr_t size;
    uint32_t enable_flag;
} loader_table;

bool loader_table_init(void);
const loader_table * loader_table_get(void);
uintptr_t loader_application_entry(void);
