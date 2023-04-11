# 2017, Aurelio Colosimo, <aurelio@aureliocolosimo.it>
# MIT License

BIGOBJ = firmware.o
LIB = firmware.a
ELF = firmware.elf
BIN = firmware.bin

# Cross compiling configuration
CROSS_COMPILE = arm-none-eabi-
CC              = $(CROSS_COMPILE)gcc
AR              = $(CROSS_COMPILE)ar
LD              = $(CROSS_COMPILE)ld
OBJCOPY         = $(CROSS_COMPILE)objcopy
STRIP           = $(CROSS_COMPILE)strip

CFLAGS += -mthumb -Wall -Werror -O0 -mcpu=cortex-m3 -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard -ggdb -g3

INCFLAGS = 
    -Isrc/Core
    -Isrc/Core/Inc
    -Isrc/Core/Src
    -Isrc/Core/Startup
    -Isrc/Drivers
    -Isrc/Drivers/CMSIS
    -Isrc/Drivers/CMSIS/Device
    -Isrc/Drivers/CMSIS/Device/ST
    -Isrc/Drivers/CMSIS/Device/ST/STM32F4xx
    -Isrc/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    -Isrc/Drivers/CMSIS/Device/ST/STM32F4xx/Source
    -Isrc/Drivers/CMSIS/Include
    -Isrc/Drivers/STM32F4xx_HAL_Driver
    -Isrc/Drivers/STM32F4xx_HAL_Driver/Inc
    -Isrc/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    -Isrc/Drivers/STM32F4xx_HAL_Driver/Src
    -Isrc/FATFS
    -Isrc/FATFS/App
    -Isrc/FATFS/Target
    -Isrc/Middlewares
    -Isrc/Middlewares/Third_Party
    -Isrc/Middlewares/Third_Party/FatFs
    -Isrc/Middlewares/Third_Party/FatFs/src
    -Isrc/Middlewares/Third_Party/FatFs/src/option
    -Isrc/stm32-bootloader
    
#OBJS += source/ff.o
#OBJS += source/ffunicode.o
#OBJS += source/ffsystem.o
#OBJS += source/diskio_stm32f4xx.o
#
#DEMO_OBJS += kim/init.o
#DEMO_OBJS += kim/kprint.o
#
#DEMO_OBJS += main.o

#all: $(BIN)
#
#%.o: %.c
#	$(CC) -c $(CFLAGS) $(INCFLAGS) -o $@ $<
#
#$(BIGOBJ): $(OBJS)
#	$(LD) $(OBJS) -r -o $(BIGOBJ)
#
#$(LIB): $(BIGOBJ)
#	$(AR) rs $(LIB) $(BIGOBJ)
#
#$(ELF): $(LIB) $(DEMO_OBJS)
#	$(LD) $(DEMO_OBJS) $(LIB) -Tkim/stm32f4xx.ld -o $(ELF)
#
#$(BIN): $(ELF)
#	$(OBJCOPY) -O binary $(ELF) $(BIN)
  
SRC     := ./src
SRCS    := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))  
  
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
    $(CC) $(CFLAGS) -c $< -o $@
    
$(ELF): $(OBJ)
	$(LD) $(OBJ) -TSTM32F103ZETX_FLASH.ld -o $(ELF)    
    
clean:
	rm -f $(OBJS) $(DEMO_OBJS) $(BIGOBJ) $(LIB) $(ELF) $(BIN)

flash: $(BIN)
	st-flash write $(BIN) 0x8000000

.PHONY: clean
