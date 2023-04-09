## STM32F407VE bootloader using SDIO interface

Functionality:

    - Copies image from the file firmware.bin to FLASH
    - Loading starts at 0x0800 8000 (can be changed)
    - Write protection is automatically removed
    - File/image checksum/crc checking is not done
    - UART1 @115200 can be used to monitor the boot process.
    - FAT32 file system with 512 - 4096 byte AUs.

Load point is set by APP_ADDRESS in bootloader.h.

Image filename is set by CONF_FILENAME in main.h.

Target card was a STM32_F4VE.

Pinout is at:  https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0.html

This code is based on:

    bootloader and main routines:

        https://akospasztor.github.io/stm32-bootloader

    FAT system, low level routines & print routines:

        https://github.com/colosimo/fatfs-stm32

The low level routines & print routines significantly reduced the image size
vs. using STM32CubeIDE generated code (15,900 vs. 22,500). That allows The lowest 
load point to be the beginning of sector 1 (0x0800 4000).

EXFAT support can be enabled.  It has been successfully tested.  The drawback with
enabling EXFAT (and the required LFN support) is the huge image size.  With EXFAT
the lowest load point is 0x0802 0000.

## Hardware notes:

SDIO is used in low speed 1 bit wide polling mode.  High speed 4 bit wide mode worked for cards with 512 byte allocation units but not with 4096 byte allocation units.

This board does NOT have a hardware "SD card is present" pin so some code was commented out.

Erasing is done via sectors. All sectors that don't have the boot loader image in it are erased. 

APP_ADDRESS can be set to any 512 byte aligned address in any erased sector.

## Building the image:

I used platformio within VSCode.  You'll need to set the workspace to the top
directory.  In the terminal window set the directory to the same one as the workspace.
Use the command "platformio run" command to build the image.  


## Porting to another processor:

Porting to the STM32F407ZG is very easy.  The only difference is the
larger FLASH.  You'll need to modify the FLASH defines to add the larger
number of sectors. 

Porting to other processors requires looking at the sector layout, erase
mechanisms and FLASH programming mechanisms.
