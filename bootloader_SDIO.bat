cd C:\work\debug\ARM_tools
copy C:\Users\bobku\Documents\GitHub\bootloader_SDIO\.pio\build\bootloader_SDIO_STM32F446ZET6\firmware.elf
.\objdump.exe -d -S -l -C -t firmware.elf >C:\Users\bobku\Documents\GitHub\bootloader_SDIO\ELF_human_readable.txt