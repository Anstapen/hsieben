
#include "ConsoleTask.h"
#include "stm32h747i_discovery.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "ICMS.h"

void Tasks::ConsoleTask(void *pvParameters)
{
    /* Tell the compiler that we do not use this for now */
    (void)pvParameters;

    /* The ICMS Module has to be initialized */
    configASSERT(ICMS::IsInitialized());

    /* Simple Task structure */
    for (;;)
    {
        /* For now, we just do a simple LED Toggle to show that the task is doing something */
        BSP_LED_Toggle(LED1);

        /* arbitriary value, does not matter at the moment */
        uint32_t value = 5;

        /* Then we send a message to the M4 Core via the FreeRTOS message buffer */
        xMessageBufferSend(ICMS::GetMessageBuffer(), (void *) &value, sizeof(uint32_t), portMAX_DELAY);

        /* Then we go to sleep for some time */
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}