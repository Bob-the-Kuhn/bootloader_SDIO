# 2017, Aurelio Colosimo, <aurelio@aureliocolosimo.it>
# MIT License

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
CFLAGS += -mthumb -Wall -mcpu=cortex-m3 


INCFLAGS +=	 -Isrc/Core
INCFLAGS +=	 -Isrc/Core/Inc
INCFLAGS +=	 -Isrc/Core/Src
INCFLAGS +=	 -Isrc/Core/Startup
INCFLAGS +=	 -Isrc/Drivers
INCFLAGS +=	 -Isrc/Drivers/CMSIS
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Device
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Device/ST
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Device/ST/STM32F1xx
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Device/ST/STM32F1xx/Include
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Device/ST/STM32F1xx/Source
INCFLAGS +=	 -Isrc/Drivers/CMSIS/Include
INCFLAGS +=	 -Isrc/Drivers/STM32F1xx_HAL_Driver
INCFLAGS +=	 -Isrc/Drivers/STM32F1xx_HAL_Driver/Inc
INCFLAGS +=	 -Isrc/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
INCFLAGS +=	 -Isrc/Drivers/STM32F1xx_HAL_Driver/Src
INCFLAGS +=	 -Isrc/FATFS
INCFLAGS +=	 -Isrc/FATFS/App
INCFLAGS +=	 -Isrc/FATFS/Target
INCFLAGS +=	 -Isrc/Middlewares
INCFLAGS +=	 -Isrc/Middlewares/Third_Party
INCFLAGS +=	 -Isrc/Middlewares/Third_Party/FatFs
INCFLAGS +=	 -Isrc/Middlewares/Third_Party/FatFs/src
INCFLAGS +=	 -Isrc/Middlewares/Third_Party/FatFs/src/option
INCFLAGS +=	 -Isrc/stm32-bootloader
	
ASSEMBLE += src/Core/Startup/startup_stm32f103zetx.s
OBJS += src/Core/Src/main.c
OBJS += src/Core/Src/stm32f1xx_hal_msp.c
OBJS += src/Core/Src/stm32f1xx_it.c
OBJS += src/Core/Src/syscalls.c
OBJS += src/Core/Src/sysmem.c
OBJS += src/Core/Src/system_stm32f1xx.c
OBJS += src/FATFS/App/fatfs.c
OBJS += src/FATFS/Target/bsp_driver_sd.c
OBJS += src/Middlewares/Third_Party/FatFs/src/diskio.c
OBJS += src/Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c
OBJS += src/Middlewares/Third_Party/FatFs/src/ff.c
OBJS += src/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c
OBJS += src/Middlewares/Third_Party/FatFs/src/option/syscall.c
OBJS += src/stm32-bootloader/bootloader.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
OBJS += src/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.c

all: $(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCFLAGS) -o $@ $<
  
%.o: %.a
	$(AS) -a $(CFLAGS) $(INCFLAGS) -o $@ $<

$(BIGOBJ): $(OBJS)
	$(LD) $(OBJS) -r -o $(BIGOBJ)

$(LIB): $(BIGOBJ)
	$(AR) rs $(LIB) $(BIGOBJ)

$(ELF): $(LIB) 
	$(LD) $(LIB) -TSTM32F103ZETX_FLASH.ld -o $(ELF)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

clean:
	rm -f $(OBJS) $(DEMO_OBJS) $(LIB) $(ELF) $(BIN)

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: clean



