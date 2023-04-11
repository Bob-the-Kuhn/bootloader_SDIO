# 2017, Aurelio Colosimo, <aurelio@aureliocolosimo.it>
# MIT License

# Windows delete and copy commands are strange
RM = del /Q /F
CP = copy /Y
#RM = rm -rf
#CP = cp -f


BIGOBJ = firmware.o
LIB = firmware.a
ELF = firmware.elf
BIN = firmware.bin

# Cross compiling configuration
CROSS_COMPILE = arm-none-eabi-
CC			  = $(CROSS_COMPILE)gcc
AR			  = $(CROSS_COMPILE)ar
AS			  = $(CROSS_COMPILE)as
LD			  = $(CROSS_COMPILE)ld
OBJCOPY		 = $(CROSS_COMPILE)objcopy
STRIP		   = $(CROSS_COMPILE)strip

#CFLAGS += -mthumb -Wall -Werror -O0 -mcpu=cortex-m3 -ggdb -g3
CFLAGS += -mthumb -Wall -mcpu=cortex-m3 -fmax-errors=5 -DMCU_STM32F103ZE -DSTM32F1 -O0 -ggdb -g3


INCFLAGS += -Isrc/Core/Inc
INCFLAGS += -Isrc/Core/Src
INCFLAGS += -Isrc/Core/Startup
INCFLAGS += -Isrc/Drivers/CMSIS/Device
INCFLAGS += -Isrc/Drivers/CMSIS/Device/ST/STM32F1xx/Include
INCFLAGS += -Isrc/Drivers/CMSIS/Device/ST/STM32F1xx/Source
INCFLAGS += -Isrc/Drivers/CMSIS/Include
INCFLAGS += -Isrc/Drivers/STM32F1xx_HAL_Driver/Inc
INCFLAGS += -Isrc/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
INCFLAGS += -Isrc/Drivers/STM32F1xx_HAL_Driver/Src
INCFLAGS += -Isrc/FATFS/App
INCFLAGS += -Isrc/FATFS/Target
INCFLAGS += -Isrc/Middlewares/Third_Party/FatFs/src
INCFLAGS += -Isrc/Middlewares/Third_Party/FatFs/src/drivers
INCFLAGS += -Isrc/Middlewares/Third_Party/FatFs/src/option
INCFLAGS += -Isrc/stm32-bootloader
	
OBJS += src/Core/Startup/startup_stm32f103zetx.o
OBJS += src/Core/Src/main.o
OBJS += src/Core/Src/stm32f1xx_hal_msp.o
OBJS += src/Core/Src/stm32f1xx_it.o
OBJS += src/Core/Src/syscalls.o
OBJS += src/Core/Src/sysmem.o
OBJS += src/Core/Src/system_stm32f1xx.o
OBJS += src/FATFS/App/fatfs.o
OBJS += src/FATFS/Target/bsp_driver_sd.o
OBJS += src/Middlewares/Third_Party/FatFs/src/diskio.o
OBJS += src/Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.o
OBJS += src/Middlewares/Third_Party/FatFs/src/ff.o
OBJS += src/Middlewares/Third_Party/FatFs/src/ff_gen_drv.o
OBJS += src/Middlewares/Third_Party/FatFs/src/option/syscall.o
OBJS += src/stm32-bootloader/bootloader.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.o
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.o

#all:
#	gcc -c src/Core/Src/main.c $(INCFLAGS) -o src/Core/Src/main.o

all: $(BIN)

%.o: %.c
	@echo Compiling $<...
	$(CC) $(CFLAGS) $(INCFLAGS) -o $@ -c $<
  
%.o: %.a
	$(AS) -a $(CFLAGS) $(INCFLAGS) -o $@ $<

$(BIGOBJ): $(OBJS)
	$(CC) $(OBJS) -r -o $(BIGOBJ)

$(LIB): $(BIGOBJ)
	$(AR) rs $(LIB) $(BIGOBJ)

$(ELF): $(LIB) 
	$(CC) $(LIB) -TSTM32F103ZETX_FLASH.ld -o $(ELF)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

clean:
	-$(RM) *.o  

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: clean



