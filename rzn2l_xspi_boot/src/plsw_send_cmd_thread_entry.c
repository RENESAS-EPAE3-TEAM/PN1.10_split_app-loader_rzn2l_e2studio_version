#include "plsw_send_cmd_thread.h"
#include "plsw_data.h"

#if EXAMPL_DEV_CONFIG_VERSION != 5      // suspend the task

/**
 * \brief                       Plsw Tx Thread entry function
 *
 * \param[in]   pvParameters    pvParameters contains TaskHandle_t
 */
void plsw_send_cmd_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    vTaskSuspend(NULL);
}

#else

/* Synchronization handles for UART TX */
extern TaskHandle_t plsw_send_cmd_thread;

/**
 * \brief                       Send start request acknowledgment
 *
 * \retval      PNIO_NOT_OK     buffer is full
 * \retval      PNIO_OK         successfully pushed 
 */
PNIO_UINT8 PLSW_txcmd_start_ack(void)
{
    PNIO_UINT8 cmd[PLSW_CMD_MINLEN], idx=0;
    cmd[idx++] = (PLSW_CMD_START_ACK&0x00FF) >> 0;
    cmd[idx++] = (PLSW_CMD_START_ACK&0xFF00) >> 8;
    cmd[idx++] = 0x00;
    cmd[idx++] = 0x00;
#ifdef PrintfDebug
    printeth("UTX start ack");
#endif
    if (PNIO_OK == PLSW_utx_push_multi(cmd, idx))
    {
        xTaskNotifyGive(plsw_send_cmd_thread);
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Send F-Parameter
 *
 * \param[in]   pBuf            Pointer to the F-Parameter buffer
 *
 * \retval      PNIO_NOT_OK     buffer is full
 * \retval      PNIO_OK         successfully pushed 
 */
PNIO_UINT8 PLSW_txcmd_fpram(PNIO_UINT8* pBuf)
{
    PNIO_UINT8 idx=0, cmd[F_PAR_LEN + PLSW_CMD_MINLEN];
    cmd[idx++] = (PLSW_CMD_FPAR&0x00FF) >> 0;
    cmd[idx++] = (PLSW_CMD_FPAR&0xFF00) >> 8;
    cmd[idx++] = F_PAR_LEN;
    cmd[idx++] = 0x00;
    OsMemCpy(&cmd[idx], pBuf, F_PRM_LEN);
    idx += F_PRM_LEN;
#ifdef PrintfDebug
    printeth("UTX F-Param");
#endif
    if (PNIO_OK == PLSW_utx_push_multi(cmd, idx))
    {
        xTaskNotifyGive(plsw_send_cmd_thread);
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Send run request
 *
 * \retval      PNIO_NOT_OK     buffer is full
 * \retval      PNIO_OK         successfully pushed 
 */
PNIO_UINT8 PLSW_txcmd_run(void)
{
    PNIO_UINT8 cmd[PLSW_CMD_MINLEN], idx=0;
    cmd[idx++] = (PLSW_CMD_RUN_REQ&0x00FF) >> 0;
    cmd[idx++] = (PLSW_CMD_RUN_REQ&0xFF00) >> 8;
    cmd[idx++] = 0x00;
    cmd[idx++] = 0x00;
#ifdef PrintfDebug
    printeth("UTX Run req");
#endif
    if (PNIO_OK == PLSW_utx_push_multi(cmd, idx))
    {
        xTaskNotifyGive(plsw_send_cmd_thread);
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Send stop request
 *
 * \retval      PNIO_NOT_OK     buffer is full
 * \retval      PNIO_OK         successfully pushed 
 */
PNIO_UINT8 PLSW_txcmd_stop(void)
{
    PNIO_UINT8 cmd[PLSW_CMD_MINLEN], idx=0;
    cmd[idx++] = (PLSW_CMD_STOP_REQ&0x00FF) >> 0;
    cmd[idx++] = (PLSW_CMD_STOP_REQ&0xFF00) >> 8;
    cmd[idx++] = 0x00;
    cmd[idx++] = 0x00;
#ifdef PrintfDebug
    printeth("UTX Stop req");
#endif
    if (PNIO_OK == PLSW_utx_push_multi(cmd, idx))
    {
        xTaskNotifyGive(plsw_send_cmd_thread);
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Send Output PDU
 *
 * \param[in]   pBuf            Pointer to the Output PDU buffer
 *
 * \retval      PNIO_NOT_OK     buffer is full
 * \retval      PNIO_OK         successfully pushed 
 */
PNIO_UINT8 PLSW_txcmd_outpdu(PNIO_UINT8* pBuf)
{
    PNIO_UINT8 idx=0, cmd[OUT_PDU_LEN + PLSW_CMD_MINLEN];
    
    cmd[idx++] = (PLSW_CMD_TX_OUTPDU&0x00FF) >> 0;
    cmd[idx++] = (PLSW_CMD_TX_OUTPDU&0xFF00) >> 8;
    cmd[idx++] = OUT_PDU_LEN;
    cmd[idx++] = 0x00;
    OsMemCpy(&cmd[idx], pBuf, OUT_PDU_LEN);
    idx += OUT_PDU_LEN;
    if (PNIO_OK == PLSW_utx_push_multi(cmd, idx))
    {
        xTaskNotifyGive(plsw_send_cmd_thread);
        return PNIO_OK;
    }
    return PNIO_NOT_OK;
}

/**
 * \brief                       Plsw Tx Thread entry function
 *
 * \param[in]   pvParameters    pvParameters contains TaskHandle_t
 */
void plsw_send_cmd_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    PNIO_UINT8 txbuf[PLSW_CMD_MAX_LEN], txlen;
    
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    // Wait for initialization
    while(1)
    {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) > 0)
        {
            if (PNIO_OK == PLSW_utx_pop(&txlen))                // Get data length
            {
                if (PNIO_OK == PLSW_utx_pop_multi(&txbuf[0], txlen))
                {
                    R_SCI_UART_Write(&g_uart1_ctrl, &txbuf[0], txlen);          // Transmit data
                }
            }
        }
    }
}

#endif
