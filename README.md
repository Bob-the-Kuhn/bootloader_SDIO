## STM32F103ZE bootloader using SDIO interface

Functionality:

    - Copies image from the file firmware.bin to FLASH
    - Loading starts at 0x0800 8000 (can be changed)
    - Write protection is automatically removed and restored
    - File/image checksum/crc checking is not done
    - UART1 @115200 can be used to monitor the boot process.
    - FAT32 file system with 512 - 4096 byte AUs. exFAT can be enabled

Load point is set by APP_ADDRESS in bootloader.h.

Image filename is set by CONF_FILENAME in main.h.

To enable sxFAT support, go to the file ffconf.h and set _FS_EXFAT to 1 and set _USE_LFN to 1.  This increases the size of the image enough that the lowest point at which an application can be loaded is affected.  See the Hardware notes section for details.

Target card was a F103ZE_Pro (has various names such as F103ZE_board) .

Pinout is at:  https://stm32-base.org/boards/STM32F103ZET6-F103ZE-Board.html

This code is based on the bootloader and main routines from:
  https://akospasztor.github.io/stm32-bootloader
  
FAT system, low level routines & print routines:
  https://github.com/colosimo/fatfs-stm32

The reset handler modifications are based on Piranna's comment in the following page:
  https://community.st.com/s/question/0D50X0000AFpTmUSQV/using-nvicsystemreset-in-bootloaderapplication-jumps

The low level routines & print routines significantly reduced the image size
vs. using STM32CubeIDE generated code (15,900 vs. 22,500). That allows The lowest 
load point to be the beginning of sector 1 (0x0800 4000).

## Building the image:

I used platformio within VSCode.  You'll need to set the workspace to the top
directory.  In the terminal window set the directory to the same one as the workspace.
Use the command "platformio run" command to build the image.  


## Hardware notes:

SDIO is used in low speed 1 wide polling mode.  Going with higher speeds or 4 bit wide mode resulted in disk I/O errors.

###To get it to boot out of FLASH reliably, R31 was replaced with a wire and pins 57 & 55 on J2 were jumpered.###

USART1 drives the CH340 USB chip (left side USB port).

Erasing is done via pages. All pages that don't have the boot loader image in it are erased.

This board does NOT have a hardware "SD card is present" pin so some code was commented out.

This program comes in at about 21,700 bytes which means it fills up pages 1-9 and extends in to page 10.  That means the lowest load point is the beginning of page 11 (0x0800 5800). Enabling exFAT support pushes the lowest load point to 0x0800 7800.

APP_ADDRESS can be set to any 512 byte aligned address in any erased page.

Changing the write protection status of the page actually occurs during hardware reset.  The code initiates the reset sequence.

## Software notes:

The software is a state machine.  There are two main state cycles:
    a) Bootloader mode vs. application mode
    b) Write protection

The flags/variables used to control the state machine have to survive the CPU going through reset and the reset handler.  This is accomplished via:
    a) A custom reset handler
    b) Some of the flags/variables are used directly by the custom reset handler.  These can go into the standard bss and data sections because they are used before the bss section of RAM gets zeroed out.
    c) Some of the flags/variables are used in the main C routines. Some can be initialized on reset.  These can go into the standard data section.  The ones that can't be initialized are put into the "no_init" section.  Allowing them to go into the bss section would result in them getting zeroed out by the reset handler.

Interactions/dependencies between the bootloader and the application are minimized by a custom reset handler. Sending the CPU through a reset between finishing the bootloader activities and starting the application means that the two can have independent startup code.  Not going through reset means, trying to put the CPU back into a state that doesn't interfere with the application, which is, to be polite, "problematic". Thanks to Piranna for showing a better method.

## Porting to another processor:

Porting to STM32F103xC and STM32F103xD chips is very easy.  The only difference is the size of the FLASH and SRAM.  You'll need to modify the FLASH defines.

Porting to other processors requires looking at the page layout, erase mechanisms, protect mechanisms and FLASH programming mechanism.