PROJECT_NAME := CM7_Firmware
APPLICATION_TYPE := EMBEDDED
TOOLCHAIN_PREFIX ?= arm-none-eabi-
LD_SCRIPT = ./ld/STM32H747XIHX_FLASH.ld

ifneq ($(DEBUG), 1)
OPT_FLAGS = -O2 -ffunction-sections -fdata-sections -fstack-usage
else
OPT_FLAGS = -O0 -g -ffunction-sections -fdata-sections -fstack-usage
endif

ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

ERROR_FLAGS = -Wall

CFLAGS = -std=gnu11 $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS)

CXXFLAGS = -std=gnu++17 $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS) -fno-exceptions -fno-rtti --specs=nano.specs

ASFLAGS = -x assembler-with-cpp $(ARCH_FLAGS)

LDFLAGS = -T$(LD_SCRIPT) -Xlinker --gc-sections --specs=nano.specs -Wl,--print-memory-usage \
-fno-rtti -fno-exceptions -static $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS)

SYMBOLS = DEBUG CORE_CM7 USE_HAL_DRIVER STM32H747xx USE_IOEXPANDER USE_STM32H747I_EVAL

LD_FIRST_OBJECTS = $(BUILD_ROOT_PATH)/main/startup_stm32h747xihx.o

-include ./env.mk
include ../../../cpmk/project.mk

.PHONY += flash
