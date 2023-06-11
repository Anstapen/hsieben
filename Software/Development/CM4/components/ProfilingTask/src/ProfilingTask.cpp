
#include "ProfilingTask.h"
#include "stm32h747i_discovery.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ICMS.h"
#include "message_buffer.h"

void Tasks::ProfilingTask(void *pvParameters)
{
    /* the function parameter does not get used for now */
    (void)pvParameters;

    /* Simple Task structure */
    for (;;)
    {
        //xMessageBufferReceive(ICMS::GetMessageBuffer(), (void*)&value, sizeof(uint32_t), pdMS_TO_TICKS(100));
        /* For now, we just do a simple LED Toggle to show that the task is doing something */
        BSP_LED_Toggle(LED3);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}