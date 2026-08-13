#include "plsw_parse_cmd_thread.h"
#include "plsw_data.h"

#if EXAMPL_DEV_CONFIG_VERSION != 5

/**
 * \brief                       Plsw Rx Thread entry function
 *
 * \param[in]   pvParameters    pvParameters contains TaskHandle_t
 */
void plsw_parse_cmd_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    vQueueDelete(psd_queue);
    xTimerDelete(plsw_cmd_timer, 0);
    xTimerDelete(plsw_urx_timer, 0);
    vTaskSuspend(NULL);
}

void plsw_uart_rx_callback(void)
{
}

void plsw_urx_timer_callback(TimerHandle_t xTimer)
{
    FSP_PARAMETER_NOT_USED(xTimer);
}

void plsw_cmd_timer_callback(TimerHandle_t xTimer)
{
    FSP_PARAMETER_NOT_USED(xTimer);
}

#else

#ifdef PrintfDebug
#include <stdio.h>
extern void printeth(char*);
#endif

// *------------ local data  ----------*
/* UART buffers */
PNIO_UINT8 urx_ring[PLSW_URX_RINGSIZE];
volatile PNIO_UINT16 urx_head = 0;
volatile PNIO_UINT16 urx_tail = 0;
volatile PNIO_UINT8 urx_empt = 1;
volatile PNIO_UINT8 urx_full = 0;
PNIO_UINT8 utx_ring[PLSW_UTX_RINGSIZE];
volatile PNIO_UINT16 utx_head = 0;
volatile PNIO_UINT16 utx_tail = 0;
volatile PNIO_UINT8 utx_empt = 1;
volatile PNIO_UINT8 utx_full = 0;
uint8_t irq_data;

/* Command context */
PLSW_RXCONTEXT rxcontext;
PNIO_UINT16 data_len_cur;

// *------------ external data  ----------*
extern TaskHandle_t plsw_parse_cmd_thread;
extern TaskHandle_t plsw_send_cmd_thread;

// *------------ internal functions  ----------*
static void plsw_rxcmd_start_req(void);
static void plsw_rxcmd_fparack(void);
static void plsw_rxcmd_inpdu(void);
static void plsw_parse_cmd(PNIO_UINT8);

/**
 * \brief       Clear UART buffer 
 */
void PLSW_clear_uart_buf(void)
{
    urx_head = 0;
    urx_tail = 0;
    urx_empt = 1;
    urx_full = 0;
    utx_head = 0;
    utx_tail = 0;
    utx_empt = 1;
    utx_full = 0;
}

/**
 * \brief       Clear command context
 */
void PLSW_clear_context(void)
{
    data_len_cur = 0;
    rxcontext.state = PLSW_STATE_CMDLSB;
    rxcontext.data_len = 0;
    rxcontext.cmd_code = 0;
}

/**
 * \brief                       Initialize process
 * 
 * \retval      PNIO_OK         UART initialization successful
 *              PNIO_NOT_OK     UART initialization failed
 */
PNIO_UINT8 PLSW_init(void)
{
    PLSW_clear_uart_buf();
    PLSW_clear_context();

    if (PLSW_URX_TIMEOUT != xTimerGetPeriod(plsw_urx_timer))
    {
        xTimerChangePeriod(plsw_urx_timer, PLSW_URX_TIMEOUT, 0);
    }
    if (PLSW_CMD_TIMEOUT != xTimerGetPeriod(plsw_cmd_timer))
    {
        xTimerChangePeriod(plsw_cmd_timer, PLSW_CMD_TIMEOUT, 0);
    }

    if (FSP_SUCCESS == R_SCI_UART_Open(&g_uart1_ctrl, &g_uart1_cfg))
    {
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Check if the UART TX buffer is empty
 * 
 * \retval      PNIO_OK         buffer is not empty
 *              PNIO_NOT_OK     buffer is empty
 */
PNIO_UINT8 PLSW_utx_exist(void)
{
    if (1 == utx_empt)  return PNIO_NOT_OK;
    else                return PNIO_OK;
}

/**
 * \brief                       Pop data from UART RX buffer
 *
 * \param[out]  pData           Pointer to store the popped data
 *
 * \retval      PNIO_NOT_OK     if the buffer is empty
 * \retval      PNIO_OK         if data is successfully popped
 */
PNIO_UINT8 PLSW_urx_pop(PNIO_UINT8 *pData)
{
    PNIO_UINT8 res = PNIO_NOT_OK;

    BSP_INTERRUPT_DISABLE
    if (0 == urx_empt)
    {
        res = PNIO_OK;
        *pData = urx_ring[urx_head++];
        urx_head &= (PLSW_URX_RINGSIZE - 1);
        if (urx_head == urx_tail)
        {
            urx_empt = 1;
        }
        urx_full = 0;
    }
    BSP_INTERRUPT_ENABLE
    return res;
}

/**
 * \brief                       Pop data from UART TX buffer
 *
 * \param[out]  pData           Pointer to store the popped data
 *
 * \retval      PNIO_NOT_OK     if the buffer is empty
 * \retval      PNIO_OK         if data is successfully popped
 */
PNIO_UINT8 PLSW_utx_pop(PNIO_UINT8 *pData)
{
    PNIO_UINT8 res = PNIO_NOT_OK;

    BSP_INTERRUPT_DISABLE
    if (0 == utx_empt)
    {
        res = PNIO_OK;
        *pData = utx_ring[utx_head++];
        utx_head &= (PLSW_UTX_RINGSIZE - 1);
        if (utx_head == utx_tail)
        {
            utx_empt = 1;
        }
        utx_full = 0;
    }
    BSP_INTERRUPT_ENABLE
    return res;
}

/**
 * \brief                       Retrieve multiple bytes from the transmit ring buffer
 *
 * \param[in]   pBuf            Pointer to the array where the retrieved data will be stored
 * \param[in]   len             Number of bytes to retrieve
 *
 * \return      PNIO_NOT_OK     Failure (buffer is empty or insufficient data)
 * \return      PNIO_OK         Success
 */
PNIO_UINT8 PLSW_utx_pop_multi(PNIO_UINT8 *pBuf, PNIO_UINT8 len)
{
    PNIO_UINT8 res = PNIO_NOT_OK;
    PNIO_UINT16 available_bytes;
    PNIO_UINT16 first_chunk;

    if (0==len || NULL==pBuf)
    {
        return res;
    }

    BSP_INTERRUPT_DISABLE
    if (0 == utx_empt)
    {
        if (utx_tail > utx_head)
        {
            available_bytes = utx_tail - utx_head;
        }
        else
        {
            available_bytes = (PNIO_UINT16)(PLSW_UTX_RINGSIZE - (utx_head-utx_tail));
        }

        if (available_bytes >= len)
        {
            res = PNIO_OK;
            if ((utx_head+len) <= PLSW_UTX_RINGSIZE)
            {
            	OsMemCpy(pBuf, &utx_ring[utx_head], len);
                utx_head = (utx_head+len) & (PLSW_UTX_RINGSIZE-1);
            }
            else
            {
                first_chunk = PLSW_UTX_RINGSIZE - utx_head;
                OsMemCpy(&pBuf[0], &utx_ring[utx_head], first_chunk);
                OsMemCpy(&pBuf[first_chunk], &utx_ring[0], len-first_chunk);
                utx_head = (len-first_chunk) & (PLSW_UTX_RINGSIZE-1);
            }
            
            if (utx_head == utx_tail)
            {
                utx_empt = 1;
            }
            utx_full = 0;
        }
    }
    BSP_INTERRUPT_ENABLE
    
    return res;
}

/**
 * \brief                       Push multiple data into UART TX buffer
 *
 * \param[in]   pBuf            Pointer to the buffer containing data to be pushed
 * \param[in]   len             Length of the data to be pushed
 *
 * \retval      PNIO_NOT_OK     if the buffer is full or data does not fit
 * \retval      PNIO_OK         if data is successfully pushed
 */
PNIO_UINT8 PLSW_utx_push_multi(PNIO_UINT8* pBuf, PNIO_UINT8 len)
{
    PNIO_UINT8 res = PNIO_OK;
    PNIO_UINT16 free_space, first_chunk, new_tail;

    BSP_INTERRUPT_DISABLE
    if (1==utx_full || len>PLSW_CMD_MAX_LEN|| 0==len || NULL==pBuf)
    {
        res = PNIO_NOT_OK;
    }
    else                                        // Check if one line fits into the buffer
    {
        if (utx_head <= utx_tail)                       // Calculate available space in buffer
        {
            free_space = (PNIO_UINT16)(PLSW_UTX_RINGSIZE - (utx_tail - utx_head));
        }
        else
        {
            free_space = utx_head - utx_tail;
        }

        if (free_space < (len+1))                       // Check if data (plus length byte) fits in available space
        {
            res = PNIO_NOT_OK;
        }
    }
    if (PNIO_NOT_OK == res)                     // If one line does not fit, throw an error
    {
        BSP_INTERRUPT_ENABLE
        return res;
    }

    utx_ring[utx_tail] = len;                   // Add the length of the data to the buffer
    new_tail = (utx_tail + 1) & (PLSW_UTX_RINGSIZE - 1);
    
    if (new_tail + len <= PLSW_UTX_RINGSIZE)    // Add data efficiently with memcpy
    {
        OsMemCpy(&utx_ring[new_tail], pBuf, len);
        new_tail = (new_tail+len) & (PLSW_UTX_RINGSIZE-1);
    }
    else
    {
        first_chunk = PLSW_UTX_RINGSIZE - new_tail;
        OsMemCpy(&utx_ring[new_tail], pBuf, first_chunk);
        OsMemCpy(&utx_ring[0], &pBuf[first_chunk], len-first_chunk);
        new_tail = (len-first_chunk) & (PLSW_UTX_RINGSIZE-1);
    }

    utx_tail = new_tail;
    if (utx_head == utx_tail)
    {
        utx_full = 1;
    }
    utx_empt = 0;

    BSP_INTERRUPT_ENABLE
    return res;
}

/**
 * \brief       Clear UART receive timeout and stop the timer
 */
void PLSW_clr_urx_timeout(void)
{
    xTimerStop(plsw_urx_timer, 0);
}

/**
 * \brief       Set command receive timeout and start the timer
 */
void PLSW_set_cmd_timeout(void)
{
    xTimerStart(plsw_cmd_timer, 0);
}

/**
 * \brief       Clear command receive timeout and stop the timer.
 */
void PLSW_clr_cmd_timeout(void)
{
    xTimerStop(plsw_cmd_timer, 0);
}

/**
 * \brief       Get the current status of command timeout timer
 *
 * \return      PNIO_TRUE if the timer is active, PNIO_NOT_OK if inactive
 *
 */
PNIO_UINT8 PLSW_get_cmd_timeout_status(void)
{
    return PNIO_OK == xTimerIsTimerActive(plsw_cmd_timer);
}

// ==================
// Internal Functions
// ==================

/**
 * \brief                       Plsw Rx Thread entry function
 *
 * \param[in]   pvParameters    pvParameters contains TaskHandle_t
 */
void plsw_parse_cmd_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    PNIO_UINT8 data;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    // Wait for initialization
    while(1)
    {
        if (0==urx_empt || ulTaskNotifyTake(pdTRUE, portMAX_DELAY)>0)
        {
            if (PNIO_OK == PLSW_urx_pop(&data))
            {
                plsw_parse_cmd(data);
            }
        }
    }
}

/**
 * \brief       Handles the receive of the start request
 */
void plsw_rxcmd_start_req(void)
{
    PSD_INTERFACE_EVENT eve = PSD_RX_START;

    if (0 == rxcontext.data_len)
    {
        events_.startupRequReceived = 1;
        xQueueSend(psd_queue, &eve, 0);
    }
}

/**
 * \brief       Handles the receive of the F-Parameter confirmation
 */
void plsw_rxcmd_fparack(void)
{
    PSD_INTERFACE_EVENT eve = PSD_RX_FPARVALID;

    if (F_PAR_VARLEN==rxcontext.data_len && 1==events_.fParamValidWait)
    {
        OsMemCpy(&fPrmValidBuf[0], &rxcontext.data[0], F_PAR_VARLEN);
        events_.fParamValidWait = 0;
        events_.fParamValidReceived = 1;
        xQueueSend(psd_queue, &eve, 0);
    }
}

/**
 * \brief       Processes the received Input PDU
 */
void plsw_rxcmd_inpdu(void)
{
    PSD_INTERFACE_EVENT eve = PSD_RX_INPDU;

    if ((IN_PDU_LEN+OUTPDU_VARLEN)==rxcontext.data_len && 1==events_.procInDatWait)
    {
        OsMemCpy(&InPDUBuf[0], &rxcontext.data[0], IN_PDU_LEN);
        OsMemCpy(&OutValidBuf[0], &rxcontext.data[IN_PDU_LEN], OUTPDU_VARLEN);
        events_.procInDatReceived = 1;
        xQueueSend(psd_queue, &eve, 0);
    }
}

/**
 * \brief               Parse received UART command and execute corresponding function
 *
 * \param[in]   data    Received byte to be parsed
 */
static void plsw_parse_cmd(PNIO_UINT8 data)
{
    PNIO_UINT8 rxcomp = 0;
    PNIO_UINT16 wk;
    
    switch (rxcontext.state)
    {
        case PLSW_STATE_CMDLSB:         // command lower byte received
            rxcontext.cmd_code = data;
            rxcontext.state++;              // proceed to receive upper byte of command
            break;

        case PLSW_STATE_CMDMSB:         // command upper byte received
            wk = data << 8;
            rxcontext.cmd_code |= wk;
            rxcontext.state++;              // proceed to receive lower byte of data length
            break;

        case PLSW_STATE_LENLSB:         // data length lower byte received
            rxcontext.data_len = data;
            rxcontext.state++;              // proceed to receive upper byte of data length
            break;

        case PLSW_STATE_LENMSB:         // data length upper byte received
            wk = data << 8;
            rxcontext.data_len |= wk;
            if (0 == rxcontext.data_len)    // receive complete (no data)
            {
                rxcomp = 1;
            }
            else                            // proceed to data receive
            {
                rxcontext.state++;
                data_len_cur = 0;
            }
            break;

        case PLSW_STATE_DATA:           // data received
            if (data_len_cur < PLSW_CMD_DATA_MAX_LEN)
            {
                rxcontext.data[data_len_cur++] = data;
            }
            if (rxcontext.data_len <= data_len_cur)
            {
                rxcomp = 1;                             // receive complete
            }
            break;

        default:
            break;
    }

    if (1 == rxcomp)            // execute command processing
    {
        PLSW_clr_urx_timeout();
        if (PLSW_CMD_START_REQ == rxcontext.cmd_code)
        {
            plsw_rxcmd_start_req();
        }
        else if (PLSW_CMD_FPAR_ACK == rxcontext.cmd_code)
        {
            plsw_rxcmd_fparack();
        }
        else if (PLSW_CMD_RX_INPDU == rxcontext.cmd_code)
        {
            plsw_rxcmd_inpdu();
        }
        PLSW_clear_context();
    }
}

/**
 * \brief UART receive interrupt handler that stores received data in the ring buffer
 *        and notifies plsw_parse_cmd_thread via semaphore.
 */
void plsw_uart_rx_callback(void)
{
    BaseType_t xTaskWoken = pdFALSE;

    SCI_UART_CFG_MULTIPLEX_INTERRUPT_ENABLE;
    FSP_CONTEXT_SAVE;           // Save context if RTOS is used 

    while (g_uart1_ctrl.p_reg->FRSR_b.R > 0)
    {
        irq_data = (uint8_t) g_uart1_ctrl.p_reg->RDR_b.RDAT;
        if (0 == urx_full)              // Push received data to the UART RX ring buffer
        {
            urx_ring[urx_tail++] = irq_data;
            urx_tail &= (PLSW_URX_RINGSIZE - 1);
            if (urx_head == urx_tail)
            {
                urx_full = 1;
            }
            urx_empt = 0;
        }
    }
    g_uart1_ctrl.p_reg->CFCLR_b.RDRFC = 1;
    vTaskNotifyGiveFromISR(plsw_parse_cmd_thread, &xTaskWoken);
    xTimerStartFromISR(plsw_urx_timer, &xTaskWoken);

    FSP_CONTEXT_RESTORE;        // Restore context if RTOS is used
    SCI_UART_CFG_MULTIPLEX_INTERRUPT_DISABLE
    portYIELD_FROM_ISR(xTaskWoken);
}

/**
 * \brief       UART callback function to handle UART events
 *
 * \param[in]   p_args: Pointer to UART callback arguments
 */
void plsw_uart_callback (uart_callback_args_t * p_args)
{
    BaseType_t xTaskWoken = pdFALSE;
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            break;
        }
        /* Ready for more data */
        case UART_EVENT_TX_DATA_EMPTY:
        {
            if (0 == utx_empt)          // indicate data is available for transmission
            {
                vTaskNotifyGiveFromISR(plsw_send_cmd_thread, &xTaskWoken);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    portYIELD_FROM_ISR(xTaskWoken);
}

/**
 * \brief               Timer callback function for timeout
 *
 * \param[in]   xTimer  Timer handle that triggered this callback
 */
void plsw_urx_timer_callback(TimerHandle_t xTimer)
{
	FSP_PARAMETER_NOT_USED(xTimer);
    PLSW_clear_context();
}

/**
 * \brief               Timer callback function for timeout
 *
 * \param[in]   xTimer  Timer handle that triggered this callback
 */
void plsw_cmd_timer_callback(TimerHandle_t xTimer)
{
	FSP_PARAMETER_NOT_USED(xTimer);
    PSD_INTERFACE_EVENT eve = PSD_CMD_TIMOUT;
    xQueueSend(psd_queue, &eve, 0);
}

#endif // EXAMPL_DEV_CONFIG_VERSION != 5
