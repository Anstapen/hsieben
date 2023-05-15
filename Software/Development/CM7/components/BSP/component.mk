SOURCE_PATHS += $(BASE_PATH)/src
EXCLUDE_SOURCE_FILES += $(BASE_PATH)/src/stm32h747i_discovery_audio.c \
						$(BASE_PATH)/src/stm32h747i_discovery_bus.c \
						$(BASE_PATH)/src/stm32h747i_discovery_camera.c \
						$(BASE_PATH)/src/stm32h747i_discovery_lcd.c \
						$(BASE_PATH)/src/stm32h747i_discovery_qspi.c \
						$(BASE_PATH)/src/stm32h747i_discovery_sd.c \
						$(BASE_PATH)/src/stm32h747i_discovery_sdram.c \
						$(BASE_PATH)/src/stm32h747i_discovery_ts.c
						


EXTRA_SOURCE_FILES += $(BASE_PATH)/src/stm32h747i_discovery.c