#include "hal_data.h"
#include "hram_sample.h"

#define BSC_PROTECT_KEY     (0xa55a0000)

#ifndef USE_HRAM
static void bsp_sdram_init (void)
{
    volatile uint32_t val;

    R_RWP_S->PRCRS  = 0x0000A50F;
    R_RWP_NS->PRCRN = 0x0000A50F;

    /* NOTE: Port setting and CKIO configuration should have been done before */
    
    // Configure clock frequency
    val = R_SYSC_NS->SCKCR;
    val &= ~(7<<16);
    //val |=  (6<<16);	// CKIO clock: 25MHz 
    val |=  (1<<16);	// CKIO clock: 66.7MHz
    R_SYSC_NS->SCKCR = val;

    /* Enable BSC and CKIO module */
    val = R_SYSC_NS->MSTPCRA;
    val &= ~(1<<0);
    R_SYSC_NS->MSTPCRA = val;
    val = R_SYSC_NS->MSTPCRA;        // dummy read: step1

    // Enable CKIO module
    val = R_SYSC_NS->MSTPCRD;
    val &= ~(1<<11);
    R_SYSC_NS->MSTPCRD = val;
    val = R_SYSC_NS->MSTPCRD;        // dummy read: step1

    R_RWP_NS->PRCRN = 0x0000A500;
    R_RWP_S->PRCRS  = 0x0000A500;

    /* Wait */
	val = R_BSC->CSnBCR[3];        // dummy read: step2
	val = R_BSC->CSnBCR[3];        // dummy read: step2
	val = R_BSC->CSnBCR[3];        // dummy read: step2
	val = R_BSC->CSnBCR[3];        // dummy read: step2
	val = R_BSC->CSnBCR[3];        // dummy read: step2


    /* SDRAM:W9825G6KH-6 */
    /* Row address: A0-A12. Column address: A0-A8. */
	val = ( 2 <<  9)	// BSZ: data bus witdh: 16-bits
	 	| ( 1 << 11)	// Reserved
	 	| ( 4 << 12)	// TYPE: SDRAM
	 	| ( 0 << 16)	// IWRRS: Idle State Insertion between Read-Read Cycles in the Same CS Space
	 	| ( 0 << 19)	// IWRRD: Idle State Insertion between Read-Read Cycles in Different CS Spaces
	 	| ( 0 << 22)	// IWRWS: Idle State Insertion between Read-Write Cycles in the Same CS Space
	 	| ( 0 << 25)	// IWRWD: Idle State Insertion between Read-Write Cycles in Different CS Spaces
	 	| ( 0 << 28);	// IWW: Idle Cycles between Write-Read Cycles and Write-Write Cycles
    R_BSC->CSnBCR[3] = val;

    /* Wait cycle */
	val = ( 2 <<  0)	// WTRC: Number of Idle states frmo REF Command/Self-refresh relase to ACTV/REF/MRS command
	 	| ( 2 <<  3)	// TRWL: Number of Auto-precharge startup Wait Cycle
	 	| ( 1 <<  7)	// A3CL: CAS Latency: 2
	 	| ( 1 << 10)	// WTRCD: Number of Waits between ACTV command and READ/WRIT command
	 	| ( 1 << 13);	// WTRP: Number of Auto-precharge completion wait states        
        R_BSC->CS3WCR_1 = val;

    /* Wait */
	val = R_BSC->CSnBCR[2];        // dummy read: step2
	val = R_BSC->CSnBCR[2];        // dummy read: step2
	val = R_BSC->CSnBCR[2];        // dummy read: step2
	val = R_BSC->CSnBCR[2];        // dummy read: step2
	val = R_BSC->CSnBCR[2];        // dummy read: step2


    /* SDRAM:W9825G6KH-6 */
    /* Row address: A0-A12. Column address: A0-A8. */
	val = ( 2 <<  9)	// BSZ: data bus witdh: 16-bits
	 	| ( 1 << 11)	// Reserved
	 	| ( 4 << 12)	// TYPE: SDRAM
	 	| ( 0 << 16)	// IWRRS: Idle State Insertion between Read-Read Cycles in the Same CS Space
	 	| ( 0 << 19)	// IWRRD: Idle State Insertion between Read-Read Cycles in Different CS Spaces
	 	| ( 0 << 22)	// IWRWS: Idle State Insertion between Read-Write Cycles in the Same CS Space
	 	| ( 0 << 25)	// IWRWD: Idle State Insertion between Read-Write Cycles in Different CS Spaces
	 	| ( 0 << 28);	// IWW: Idle Cycles between Write-Read Cycles and Write-Write Cycles
    R_BSC->CSnBCR[2] = val;

    /* Wait cycle */
    val = ( 1 <<  7)	// A3CL: CAS Latency: 2
	 	| ( 1 << 10);	// Reserved 1

    R_BSC->CS2WCR_1 = val;

    /* SDRAM control */
    R_BSC->SDCR = 0x00110811;		// auto-refresh, auto-precharge mode, Col 9-bits, Row 13-bits

    /* Refresh setting for SDRAM */
    R_BSC->RTCOR = BSC_PROTECT_KEY 
                 | ( 29 <<  0);        // Refresh Time: 29 counts
    R_BSC->RTCSR = BSC_PROTECT_KEY 
                 | (  0 <<  7)         // Compare match Flag: clear
                 | (  0 <<  6)         // Compare match interrupt enable: Disabled
                 | (  2 <<  3)         // Refresh timer count clock: CKIO/16
                 | (  0 <<  0);        // Refresh count: 1 time

    /* wait 200us*/
    R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);

    /* Power-on Sequence */
    /* Set mode register of SDRAM. needs wait for 2 SDRAM clock after set. */
    *((uint16_t *)0x80212040) = 0x0000;   // Burst length=1, Sequential, CL=2, Burst read and burst write
    *((uint16_t *)0x80211040) = 0x0000;   // Burst length=1, Sequential, CL=2, Burst read and burst write
  
}
#endif

#ifdef USE_HRAM
fsp_err_t hram_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* MDVn pin should be low(Set XSPI operating voltage at 1.8V) */
    uint32_t mdv = (R_SYSC_NS->MD_MON >> 16) & 0x1f;
    if ((0 << MDVn) != (mdv & (1 << MDVn)))
    {
        return FSP_ERR_ABORTED;
    }

	err = g_hyperbus0.p_api->open(g_hyperbus0.p_ctrl, g_hyperbus0.p_cfg);
    while(FSP_SUCCESS != err);

    /* Read HyperRAM register(The data read is big endian.) */
    /* Read setting of identification register 0 */
    volatile hram_transfer_t trans[2];
    trans[0].cmd = HRAM_DEVICE_READ_CMD;
    trans[0].addr = HRAM_DEVICE_ID0_ADDR;
    trans[0].latency = 6;
    trans[0].write = false;
    err = hram_trans(trans[0]);
    while(FSP_SUCCESS != err);
    /* Since the data is big endian, convert the endian and output. */

    /* Read setting of identification register 1 */
    trans[0].addr = HRAM_DEVICE_ID1_ADDR;
    err = hram_trans(trans[0]);
    while(FSP_SUCCESS != err);

    /* Read setting of configuration register 0 */
    trans[0].addr = HRAM_DEVICE_CFG0_ADDR;
    err = hram_trans(trans[0]);
    while(FSP_SUCCESS != err);

    /* Read setting of configuration register 1 */
    trans[0].addr = HRAM_DEVICE_CFG1_ADDR;
    err = hram_trans(trans[0]);
    while(FSP_SUCCESS != err);

    return err;
}

/*******************************************************************************************************************//**
 * Function Name: hram_trans
 * Description  : Transfer HyperRAM
 * Arguments    : hram_transfer_t trans
 * Return Value : fsp_err_t err
 **********************************************************************************************************************/
fsp_err_t hram_trans(hram_transfer_t trans)
{
    fsp_err_t err = FSP_SUCCESS;

    /* xSPI Command Manual Type Buf */
    R_XSPIn->BUF[0].CDT_b.CMDSIZE = 2;  // Command Size: 2 bytes
    R_XSPIn->BUF[0].CDT_b.ADDSIZE = 4;  // Address Size: 4 bytes
    R_XSPIn->BUF[0].CDT_b.DATASIZE = 2; // Write/Read Data Size: 2 bytes
    R_XSPIn->BUF[0].CDT_b.LATE = trans.latency; // Latency cycle
    R_XSPIn->BUF[0].CDT_b.TRTYPE = trans.write; // Transaction Type
    R_XSPIn->BUF[0].CDT_b.CMD = trans.cmd;      // Command (1-2 bytes)
    /* xSPI Command Manual Address Buf */
    R_XSPIn->BUF[0].CDA  = trans.addr;

    /* For write transactions, write data to buffer registers */
    if (true == trans.write)
    {
        R_XSPIn->BUF[0].CDD0 = trans.data;
    }

    /* xSPI Command Manual Control Register 0 */
    R_XSPIn->CDCTL0_b.CSSEL = XSPIn_CS; // Chip select
    R_XSPIn->CDCTL0_b.TRNUM = 0;  // Transaction number: Issue 1 command (using command buffer 0)
    R_XSPIn->CDCTL0_b.PERMD = 0;  // Periodic mode: Direct manual-command mode
    R_XSPIn->CDCTL0_b.PERITV = 0; // Periodic transaction interval: 2 cycles
    FSP_HARDWARE_REGISTER_WAIT(R_XSPIn->CDCTL0_b.TRREQ, 0); // Wait for transaction completion
    R_XSPIn->CDCTL0_b.TRREQ = 1;  // Transaction request: Request transaction
    /* xSPI Interrupt Status Register */
    FSP_HARDWARE_REGISTER_WAIT(R_XSPIn->INTS_b.CMDCMP, 1);  // Wait for requested manual command completion
    /* xSPI Interrupt Clear Register */
    R_XSPIn->INTC_b.CMDCMPC = 1;  // Command Completed interrupt clear: Clear interrupt status

    /* DS Timeout and AHB Bus Error Detection */
    err = check_ints();
    while(FSP_SUCCESS != err)
    {
        return err;
    }

    /* For read transactions, read data from buffer registers. */
    if (false == trans.write)
    {
        trans.data = R_XSPIn->BUF[0].CDD0;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Function Name: check_ints
 * Description  : DS Timeout and AHB Bus Error Detection
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
fsp_err_t check_ints(void)
{
    /* AHB bus error detection */
    if (R_XSPIn->INTS_b.BUSERR)
    {
        R_XSPIn->INTC_b.BUSERRC = 1;
        return FSP_ERR_ABORTED;
    }
    /* DS timeout detection */
    if (XSPIn_DSTOCS)
    {
        XSPIn_DSTOCS_C = 1;
        return FSP_ERR_ABORTED;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Function Name: swap16
 * Description  : Convert endian
 * Arguments    : uint16_t value
 * Return Value : return ret
 **********************************************************************************************************************/
uint16_t swap16(uint16_t value)
{
    uint16_t ret;
    ret  = value << 8;
    ret |= value >> 8;
    return ret;
}
#endif

void loader_external_memory_init (void)
{
#ifdef USE_HRAM
    (void) hram_init();
#else
    bsp_sdram_init();
#endif
}
