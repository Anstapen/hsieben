
#include "ProfilingTask.h"
#include "stm32h747i_discovery.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ICMS.h"
#include "message_buffer.h"

void Tasks::ProfilingTask(void *pvParameters)
{
    /* Tell the compiler that we do not use this for now */
    (void)pvParameters;

    configASSERT(ICMS::IsInitialized());

    /* Simple Task structure */
    for (;;)
    {
        uint32_t value;
        xMessageBufferReceive(ICMS::GetMessageBuffer(),&value, sizeof(uint32_t), portMAX_DELAY);
        /* For now, we just do a simple LED Toggle to show that the task is doing something */
        BSP_LED_Toggle(LED3);
    }
}