/*
 * Author: Aurelio Colosimo, 2017
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _STM32F446X_
#define _STM32F446X_

#include <cpu.h>

/* Flash Interface registers */
#define R_FLASH_ACR    reg32(0x40023c00)         
                                              
  /* RCC registers */
#define RCC_CR       reg32(0x40023800)
#define RCC_PLLCFGR  reg32(0x40023804)
#define RCC_CFGR     reg32(0x40023808)
#define RCC_AHB1ENR  reg32(0x40023830)
#define RCC_AHB2ENR  reg32(0x40023834)
#define RCC_APB1ENR  reg32(0x40023840)
#define RCC_APB2ENR  reg32(0x40023844)                                            
                                              
                                              
/* GPIO registers */      
                                               
#define R_GPIOA_MODER  reg32(0x40020000)         
#define R_GPIOA_OTYPER reg32(0x40020004)         
#define R_GPIOA_PUPDR  reg32(0x4002000c)         
#define R_GPIOA_IDR    reg32(0x40020010)         
#define R_GPIOA_BSRR   reg32(0x40020018)         
#define R_GPIOA_AFRL   reg32(0x40020020)         
#define R_GPIOA_AFRH   reg32(0x40020024)         
                                                 
/* SDIO registers */       
#define R_SDIO_POWER   reg32(0x40012c00)         
#define R_SDIO_CLKCR   reg32(0x40012c04)         
#define R_SDIO_ARG     reg32(0x40012c08)         
#define R_SDIO_CMD     reg32(0x40012c0c)         
#define R_SDIO_RESP1   reg32(0x40012c14)         
#define R_SDIO_RESP2   reg32(0x40012c18)         
#define R_SDIO_RESP3   reg32(0x40012c1c)         
#define R_SDIO_RESP4   reg32(0x40012c20)         
#define R_SDIO_DTIMER  reg32(0x40012c24)         
#define R_SDIO_DLEN    reg32(0x40012c28)         
#define R_SDIO_DCTRL   reg32(0x40012c2c)         
#define R_SDIO_DCOUNT  reg32(0x40012c30)         
#define R_SDIO_STA     reg32(0x40012c34)         
#define R_SDIO_ICR     reg32(0x40012c38)         
#define R_SDIO_FIFO    reg32(0x40012c80)         
                                                 
/* USART registers  */       
#define R_USART1_SR    reg32(0x40011000)         
#define R_USART1_DR    reg32(0x40011004)         
#define R_USART1_BRR   reg32(0x40011008)         
#define R_USART1_CR1   reg32(0x4001100c)         

#endif /* _STM32F446X_ */