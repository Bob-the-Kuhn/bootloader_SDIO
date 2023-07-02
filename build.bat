platformio run
cd C:\work\debug\ARM_tools
copy C:\Users\bobku\Documents\GitHub\bootloader_SDIO\.pio\build\bootloader_SPI_STM32H753ZI\firmware.elf
.\objdump.exe -d -S -l -C -t firmware.elf >C:\work\debug\ARM_tools\ARM_disassemble_with_line_numbers.txt
cd C:\Users\bobku\Documents\GitHub\bootloader_SDIO