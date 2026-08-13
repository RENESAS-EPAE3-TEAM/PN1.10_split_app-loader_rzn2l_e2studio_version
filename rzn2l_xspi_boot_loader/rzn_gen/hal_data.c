/* generated HAL source file - do not edit */
#include "hal_data.h"
xspi_qspi_instance_ctrl_t g_qspi_ldr_ctrl;

static const spi_flash_erase_command_t g_qspi_ldr_erase_command_list[] =
{
#if 4096 > 0
  { .command = 0x20, .size = 4096 },
#endif
#if 32768 > 0
  { .command = 0x52, .size = 32768 },
#endif
#if 65536 > 0
  { .command = 0xD8, .size = 65536 },
#endif
#if 0xC7 > 0
  { .command = 0xC7, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
#endif
        };

static xspi_qspi_timing_setting_t g_qspi_ldr_timing_settings =
{ .command_to_command_interval = XSPI_QSPI_COMMAND_INTERVAL_CLOCKS_7, .cs_pullup_lag =
          XSPI_QSPI_CS_PULLUP_CLOCKS_NO_EXTENSION,
  .cs_pulldown_lead = XSPI_QSPI_CS_PULLDOWN_CLOCKS_NO_EXTENSION };

static const xspi_qspi_extended_cfg_t g_qspi_ldr_extended_cfg =
{ .unit = 0, .chip_select = XSPI_QSPI_CHIP_SELECT_0, .memory_size = XSPI_QSPI_MEMORY_SIZE_64MB, .p_timing_settings =
          &g_qspi_ldr_timing_settings,
#if 0 == 0
  .prefetch_en = (xspi_qspi_prefetch_function_t) XSPI_QSPI_CFG_UNIT_0_PREFETCH_FUNCTION,
#else
    .prefetch_en       = (xspi_qspi_prefetch_function_t) XSPI_QSPI_CFG_UNIT_1_PREFETCH_FUNCTION,
#endif
        };
const spi_flash_cfg_t g_qspi_ldr_cfg =
{ .spi_protocol = SPI_FLASH_PROTOCOL_1S_4S_4S,
  .address_bytes = SPI_FLASH_ADDRESS_BYTES_3,
  .dummy_clocks = SPI_FLASH_DUMMY_CLOCKS_6,
  .read_command = 0xEB,
  .page_program_command = 0x02,
  .write_enable_command = 0x06,
  .status_command = 0x05,
  .write_status_bit = 0,
  .xip_enter_command = 0x20,
  .xip_exit_command = 0xFF,
  .p_erase_command_list = &g_qspi_ldr_erase_command_list[0],
  .erase_command_list_length = sizeof(g_qspi_ldr_erase_command_list) / sizeof(g_qspi_ldr_erase_command_list[0]),
  .p_extend = &g_qspi_ldr_extended_cfg, };
/** This structure encompasses everything that is needed to use an instance of this interface. */
const spi_flash_instance_t g_qspi_ldr =
{ .p_ctrl = &g_qspi_ldr_ctrl, .p_cfg = &g_qspi_ldr_cfg, .p_api = &g_spi_flash_on_xspi_qspi, };
xspi_hyper_instance_ctrl_t g_hyperbus0_ctrl;

static xspi_hyper_cs_timing_setting_t g_hyperbus0_cs_timing_settings =
{ .transaction_interval = XSPI_HYPER_TRANSACTION_INTERVAL_CLOCKS_7, .cs_pullup_lag =
          XSPI_HYPER_CS_PULLUP_CLOCKS_NO_EXTENSION,
  .cs_pulldown_lead = XSPI_HYPER_CS_PULLDOWN_CLOCKS_NO_EXTENSION, };

static xspi_hyper_extended_cfg_t g_hyperbus0_extended_cfg =
{ .unit = 0,
  .chip_select = XSPI_HYPER_CHIP_SELECT_1,
  .memory_size = XSPI_HYPER_MEMORY_SIZE_8MB,
  .data_latching_delay_clock = 0x08,
  .p_cs_timing_settings = &g_hyperbus0_cs_timing_settings,
  .p_autocalibration_preamble_pattern_addr = 0x00,
#if 0 == 0
  .prefetch_en = (xspi_hyper_prefetch_function_t) XSPI_HYPER_CFG_UNIT_0_PREFETCH_FUNCTION,
#else
    .prefetch_en                             = (xspi_hyper_prefetch_function_t) XSPI_HYPER_CFG_UNIT_1_PREFETCH_FUNCTION,
#endif
        };
const hyperbus_cfg_t g_hyperbus0_cfg =
{ .burst_type = HYPERBUS_BURST_TYPE_LINEAR,
  .access_space = HYPERBUS_SPACE_SELECT_MEMORY_SPACE,
  .read_latency_count = HYPERBUS_LATENCY_COUNT_6,
  .memory_write_latency_count = HYPERBUS_LATENCY_COUNT_6,
  .register_write_latency_count = HYPERBUS_LATENCY_COUNT_0,
  .p_extend = &g_hyperbus0_extended_cfg, };
/** This structure encompasses everything that is needed to use an instance of this interface. */
const hyperbus_instance_t g_hyperbus0 =
{ .p_ctrl = &g_hyperbus0_ctrl, .p_cfg = &g_hyperbus0_cfg, .p_api = &g_hyperbus_on_xspi_hyper, };
void g_hal_init(void)
{
    g_common_init ();
}
