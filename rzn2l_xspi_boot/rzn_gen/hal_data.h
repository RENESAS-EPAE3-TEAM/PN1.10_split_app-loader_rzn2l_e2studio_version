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
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_cmtw.h"
#include "r_timer_api.h"
FSP_HEADER
extern const spi_flash_instance_t g_qspi_ldr;
extern xspi_qspi_instance_ctrl_t g_qspi_ldr_ctrl;
extern const spi_flash_cfg_t g_qspi_ldr_cfg;
extern const hyperbus_instance_t g_hyperbus0;
extern xspi_hyper_instance_ctrl_t g_hyperbus0_ctrl;
extern const hyperbus_cfg_t g_hyperbus0_cfg;
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;

#ifndef g_uart1_tx_transfer_callback
void g_uart1_tx_transfer_callback(transfer_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart1;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart1_ctrl;
extern const uart_cfg_t g_uart1_cfg;
extern const sci_uart_extended_cfg_t g_uart1_cfg_extend;

#ifndef plsw_uart_callback
void plsw_uart_callback(uart_callback_args_t *p_args);
#endif

#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED == g_transfer0)
                #define g_uart1_P_TRANSFER_TX (NULL)
            #else
#define g_uart1_P_TRANSFER_TX (&g_transfer0)
#endif
#if (FSP_NOT_DEFINED == FSP_NOT_DEFINED)
#define g_uart1_P_TRANSFER_RX (NULL)
#else
                #define g_uart1_P_TRANSFER_RX (&FSP_NOT_DEFINED)
            #endif
#undef FSP_NOT_DEFINED
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif

#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED == FSP_NOT_DEFINED)
#define g_uart0_P_TRANSFER_TX (NULL)
#else
                #define g_uart0_P_TRANSFER_TX (&FSP_NOT_DEFINED)
            #endif
#if (FSP_NOT_DEFINED == FSP_NOT_DEFINED)
#define g_uart0_P_TRANSFER_RX (NULL)
#else
                #define g_uart0_P_TRANSFER_RX (&FSP_NOT_DEFINED)
            #endif
#undef FSP_NOT_DEFINED
extern const spi_flash_instance_t g_qspi0;
extern xspi_qspi_instance_ctrl_t g_qspi0_ctrl;
extern const spi_flash_cfg_t g_qspi0_cfg;
/** CMTW Timer Instance */
extern const timer_instance_t g_timer0;

/** Access the CMTW instance using these structures when calling API functions directly (::p_api is not used). */
extern cmtw_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
