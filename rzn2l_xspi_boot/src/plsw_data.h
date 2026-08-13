#ifndef PLSW_DATA_H_
#define PLSW_DATA_H_

#if EXAMPL_DEV_CONFIG_VERSION == 5      // suspend the task

#include "hal_data.h"
#include "limits.h"
#include "pnio_types.h"
#include "pnioerrx.h"
#include "psd_interface.h"

#define PLSW_CMD_START_REQ      0x0101  // Startup Request
#define PLSW_CMD_START_ACK      0x0102  // Startup Request Acknowledgment
#define PLSW_CMD_RUN_REQ        0x0103  // Operation Start Request
#define PLSW_CMD_STOP_REQ       0x0104  // Operation Start Request Acknowledgment
#define PLSW_CMD_TX_OUTPDU      0x0201  // Send Output PDU
#define PLSW_CMD_RX_INPDU       0x0202  // Send Input PDU
#define PLSW_CMD_FPAR           0x0204  // Send F-Parameter
#define PLSW_CMD_FPAR_ACK       0x0205  // F-Parameter Receive Confirmation

#define PLSW_URX_RINGSIZE       0x0100U // The size of the UART RX ring buffer from the Safety board
#define PLSW_UTX_RINGSIZE       0x0100U // The size of the UART TX ring buffer for the Safety board

#define PLSW_CMD_DATA_MAX_LEN   0x0082  // Maximum data size  from the Safety board
#define PLSW_CMD_MAX_LEN        (PLSW_CMD_DATA_MAX_LEN+4)

#define PLSW_CMD_MINLEN         4       // Minimum length for command
#define F_PAR_VARLEN            2       // F-Parameter validation length
#define OUTPDU_VARLEN           2       // Output PDU validation length

#define PLSW_URX_TIMEOUT        3       // [ms] Uart rx
#define PLSW_CMD_TIMEOUT        5       // [ms] Command response timeout

typedef enum PLSW_RXSTATE
{
    PLSW_STATE_CMDLSB = 0,  
    PLSW_STATE_CMDMSB,
    PLSW_STATE_LENLSB,
    PLSW_STATE_LENMSB,
    PLSW_STATE_DATA,
    PLSW_STATE_ERR
} plsw_rxst;

typedef struct PLSW_RXCONTEXT_
{
    plsw_rxst state;                            // Current status
    PNIO_UINT16 cmd_code;                       // Received command code
    PNIO_UINT16 data_len;                       // Data receive length  
    PNIO_UINT8 data[PLSW_CMD_DATA_MAX_LEN];     // Received data
    
} PLSW_RXCONTEXT;

void PLSW_clear_uart_buf(void);
void PLSW_clear_context(void);
PNIO_UINT8 PLSW_init(void);
PNIO_UINT8 PLSW_utx_exist(void);
PNIO_UINT8 PLSW_urx_pop(PNIO_UINT8*);
PNIO_UINT8 PLSW_utx_pop(PNIO_UINT8*);
PNIO_UINT8 PLSW_utx_pop_multi(PNIO_UINT8*, PNIO_UINT8);
PNIO_UINT8 PLSW_utx_push_multi(PNIO_UINT8*, PNIO_UINT8);
void PLSW_clr_urx_timeout(void);
void PLSW_set_cmd_timeout(void);
void PLSW_clr_cmd_timeout(void);
PNIO_UINT8 PLSW_get_cmd_timeout_status(void);

PNIO_UINT8 PLSW_txcmd_start_ack(void);
PNIO_UINT8 PLSW_txcmd_fpram(PNIO_UINT8*);
PNIO_UINT8 PLSW_txcmd_run(void);
PNIO_UINT8 PLSW_txcmd_stop(void);
PNIO_UINT8 PLSW_txcmd_outpdu(PNIO_UINT8*);

void PLSW_dbg_calc_clock(PNIO_UINT8);
#endif

#endif /* PLSW_DATA_H_ */
