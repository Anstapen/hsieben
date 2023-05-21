

#include "ICMS.h"
#include <memory>
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "stm32h7xx_hal.h"

#define ICMS_MEM __attribute__((section(".ICMS_section")))



void NotifyM7(void *updatedMessageBuffer)
{
    ICMS::NotifyM7(updatedMessageBuffer);
}

void NotifyM4(void *updatedMessageBuffer)
{
    ICMS::NotifyM4(updatedMessageBuffer);
}

ICMS &ICMS::Instance()
{
    static ICMS_MEM ICMS instance;
    return instance;
}

bool ICMS::Init()
{

    /* get the ICMS Object singleton */
    ICMS &instance = ICMS::Instance();

    /* Create the FreeRTOS Message Buffer for the Control Messages using the statically allocated memory. */
    instance.controlMessageBuffer = xMessageBufferCreateStatic(CONTROL_BUFFER_BYTES,
                                                               instance.controlMessageBuffer_RAW,
                                                               &instance.controlMessageBufferStruct);

    /* Create the 1st Data message buffer */
    instance.dataMessageBuffer[0] = xMessageBufferCreateStatic(DATA_BUFFER_BYTES,
                                                               &(instance.dataMessageBuffer_RAW[0][0]),
                                                               &(instance.dataMessageBufferStruct[0]));

    instance.is_initialized = true;
    return true;
}

bool ICMS::IsInitialized()
{
    return ICMS::Instance().is_initialized;
}

void ICMS::NotifyM7(void *updatedMessageBuffer)
{
    ICMS::NotifyCore(ICMS::Core::CM7, ( MessageBufferHandle_t ) updatedMessageBuffer);
}

void ICMS::NotifyM4(void *updatedMessageBuffer)
{
    ICMS::NotifyCore(ICMS::Core::CM4, ( MessageBufferHandle_t ) updatedMessageBuffer);
}

void ICMS::NotifyCore(ICMS::Core core, MessageBufferHandle_t message_buffer)
{
    ICMS::GenerateInterrupt(core);
}

void ICMS::SetInterrupts(ICMS::Core core)
{
    switch (core)
    {
    case ICMS::Core::CM4:
        HAL_NVIC_SetPriority(CM4_IRQ, 0xFU, 0U);
        HAL_NVIC_EnableIRQ(CM4_IRQ);
        break;
    case ICMS::Core::CM7:
        HAL_NVIC_SetPriority(CM7_IRQ, 0xFU, 0U);
        HAL_NVIC_EnableIRQ(CM7_IRQ);
        break;
    }
}

void ICMS::GenerateInterrupt(ICMS::Core core)
{
    switch (core)
    {
    case ICMS::Core::CM4:
        HAL_EXTI_D1_EventInputConfig(EXTI_LINE0, EXTI_MODE_IT, DISABLE);
        HAL_EXTI_D2_EventInputConfig(EXTI_LINE0, EXTI_MODE_IT, ENABLE);
        HAL_EXTI_GenerateSWInterrupt(EXTI_LINE0);
        break;
    case ICMS::Core::CM7:
        HAL_EXTI_D2_EventInputConfig(EXTI_LINE1, EXTI_MODE_IT, DISABLE);
        HAL_EXTI_D1_EventInputConfig(EXTI_LINE1, EXTI_MODE_IT, ENABLE);
        HAL_EXTI_GenerateSWInterrupt(EXTI_LINE1);
        break;
    }
}

bool ICMS::IsControlMessageBuffer(MessageBufferHandle_t message_buffer)
{
    return (message_buffer == ICMS::Instance().controlMessageBuffer);
}

void ICMS::SendReceiveCompleteISR()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferReceiveCompletedFromISR( ICMS::Instance().dataMessageBuffer[0], &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

MessageBufferHandle_t ICMS::GetMessageBuffer()
{
    return ICMS::Instance().dataMessageBuffer[0];
}