/**
 * @file ICMS.h
 * @author Anton Stapenhorst (anton@stapenhorst.org)
 * @brief Inter Core Messaging System
 * @version 0.1
 * @date 2023-05-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#pragma once

#include "FreeRTOS.h"
#include "message_buffer.h"
#include "ICMS_Config.h"

#define CM4_IRQ EXTI0_IRQn
#define CM7_IRQ EXTI1_IRQn

void NotifyM7(void *updatedMessageBuffer);
void NotifyM4(void *updatedMessageBuffer);


class ICMS
{
public:
    enum class Core {
        CM4 = 0,
        CM7
    };
private:
    static constexpr uint32_t CONTROL_BUFFER_BYTES = 512;
    static constexpr uint32_t DATA_BUFFER_BYTES = 512;
public:
    static void NotifyM7(void *updatedMessageBuffer);
    static void NotifyM4(void *updatedMessageBuffer);
    static void SetInterrupts(Core core);
    static void GenerateInterrupt(Core core);
public:
    static ICMS& Instance();
    static ICMS* InstancePTR();
    static bool Init();
    static bool IsInitialized();
    static void SendReceiveCompleteISR();
    static MessageBufferHandle_t GetMessageBuffer();
    ~ICMS(){};
private:
    ICMS(){};
    ICMS(const ICMS&) = delete;
    ICMS(ICMS&&) = delete;
    ICMS & operator=(const ICMS&) = delete;
    ICMS & operator=(ICMS&&) = delete;
private:
    static bool IsControlMessageBuffer(MessageBufferHandle_t message_buffer);
    static void NotifyCore(Core core, MessageBufferHandle_t message_buffer);
    

private:
    MessageBufferHandle_t controlMessageBuffer;
    MessageBufferHandle_t dataMessageBuffer[MAX_DATA_BUFFERS];

    StaticStreamBuffer_t controlMessageBufferStruct;
    StaticStreamBuffer_t dataMessageBufferStruct[MAX_DATA_BUFFERS];

    /* raw buffers for static allocation */
    uint8_t controlMessageBuffer_RAW[CONTROL_BUFFER_BYTES];
    uint8_t dataMessageBuffer_RAW[MAX_DATA_BUFFERS][DATA_BUFFER_BYTES];

    /* used to show if the module has been initialized */
    bool is_initialized = false;
};

