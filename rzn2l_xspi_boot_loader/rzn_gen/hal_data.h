/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_xspi_qspi.h"
#include "r_spi_flash_api.h"
#include "r_xspi_hyper.h"
#include "r_hyperbus_api.h"
FSP_HEADER
extern const spi_flash_instance_t g_qspi_ldr;
extern xspi_qspi_instance_ctrl_t g_qspi_ldr_ctrl;
extern const spi_flash_cfg_t g_qspi_ldr_cfg;
extern const hyperbus_instance_t g_hyperbus0;
extern xspi_hyper_instance_ctrl_t g_hyperbus0_ctrl;
extern const hyperbus_cfg_t g_hyperbus0_cfg;
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
