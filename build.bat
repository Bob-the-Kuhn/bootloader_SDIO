platformio run -e bootloader_SDIO_STM32F746ZGTX
cd C:\work\debug\ARM_tools
copy C:\work\temp\bootloader_platformio\bootloader_SDIO\.pio\build\bootloader_SDIO_STM32F746ZGTX\firmware.elf
.\objdump.exe -d -S -l -C -t firmware.elf >C:\work\debug\ARM_tools\ARM_disassemble_with_line_numbers.txt
cd C:\work\temp\bootloader_platformio\bootloader_SDIO