
#include "ProfilingTask.h"
#include "stm32h747i_discovery.h"
#include "FreeRTOS.h"
#include "task.h"

void Tasks::ProfilingTask(void *pvParameters)
{
    /* Tell the compiler that we do not use this for now */
    (void)pvParameters;

    /* Simple Task structure */
    for (;;)
    {
        /* For now, we just do a simple LED Toggle to show that the task is doing something */
        BSP_LED_Toggle(LED3);

        /* Then we go to sleep for some time */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}