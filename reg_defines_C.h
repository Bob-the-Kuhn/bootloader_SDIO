
R_NVIC_ISER  0xE000EF00
R_NVIC_ISPR

R_SYST_CSR
R_SYST_CVR 
R_SYST_RVR 








/* Flash Interface registers */
#define R_FLASH_ACR    reg32(0x40023c00)         
                                               
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

