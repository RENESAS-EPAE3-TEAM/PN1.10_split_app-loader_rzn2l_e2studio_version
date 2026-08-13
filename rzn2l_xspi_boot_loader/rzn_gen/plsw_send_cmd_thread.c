/* generated thread source file - do not edit */
#include "plsw_send_cmd_thread.h"

#if 1
static StaticTask_t plsw_send_cmd_thread_memory;
#if defined(__ARMCC_VERSION)           /* AC6 compiler */
                static uint8_t plsw_send_cmd_thread_stack[0x0500] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #else
static uint8_t plsw_send_cmd_thread_stack[0x0500] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.plsw_send_cmd_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#endif
#endif
TaskHandle_t plsw_send_cmd_thread;
void plsw_send_cmd_thread_create(void);
static void plsw_send_cmd_thread_func(void *pvParameters);
void rtos_startup_err_callback(void *p_instance, void *p_data);
void rtos_startup_common_init(void);
extern uint32_t g_fsp_common_thread_count;

const rm_freertos_port_parameters_t plsw_send_cmd_thread_parameters = {
		.p_context = (void*) NULL, };

void plsw_send_cmd_thread_create(void) {
	/* Increment count so we will know the number of threads created in the FSP Configuration editor. */
	g_fsp_common_thread_count++;

	/* Initialize each kernel object. */

#if 1
	plsw_send_cmd_thread = xTaskCreateStatic(
#else
                    BaseType_t plsw_send_cmd_thread_create_err = xTaskCreate(
                    #endif
			plsw_send_cmd_thread_func, (const char*) "Plsw Tx Thread",
			0x0500 / 4, // In words, not bytes
			(void*) &plsw_send_cmd_thread_parameters, //pvParameters
			27,
#if 1
			(StackType_t*) &plsw_send_cmd_thread_stack,
			(StaticTask_t*) &plsw_send_cmd_thread_memory
#else
                        & plsw_send_cmd_thread
                        #endif
			);

#if 1
	if (NULL == plsw_send_cmd_thread) {
		rtos_startup_err_callback(plsw_send_cmd_thread, 0);
	}
#else
                    if (pdPASS != plsw_send_cmd_thread_create_err)
                    {
                        rtos_startup_err_callback(plsw_send_cmd_thread, 0);
                    }
                    #endif
}
static void plsw_send_cmd_thread_func(void *pvParameters) {
	/* Initialize common components */
	rtos_startup_common_init();

	/* Initialize each module instance. */

	/* Enter user code for this thread. Pass task handle. */
	plsw_send_cmd_thread_entry(pvParameters);
}
