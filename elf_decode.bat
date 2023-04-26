cd C:\work\debug\ARM_tools
copy C:\Users\bobku\STM32CubeIDE\workspace_1.10.1\SDIO_stm32f103zet_Black\Debug\SDIO_stm32f103zet_Black.elf
objdump.exe -d -S -l -C -t firmware.elf >C:\work\debug\ARM_tools\ARM_disassemble_with_line_numbers.txt