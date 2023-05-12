// from:  https://community.st.com/s/question/0D50X00009XkhN8SAJ/stm32f103-option-byte-pgerr


FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data)
{
 FLASH_Status status = FLASH_COMPLETE;
 /* Check the parameters */
 assert_param(IS_OB_DATA_ADDRESS(Address));
 status = FLASH_WaitForLastOperation(ProgramTimeout);
 if(status == FLASH_COMPLETE)
 {
 /* Authorize the small information block programming */
 FLASH->OPTKEYR = FLASH_KEY1;
 FLASH->OPTKEYR = FLASH_KEY2;
 /* Enables the Option Bytes Programming operation */
 FLASH->CR |= CR_OPTPG_Set;
 *(__IO uint16_t*)Address = Data;
 /* Wait for last operation to be completed */
 status = FLASH_WaitForLastOperation(ProgramTimeout);
 if(status != FLASH_TIMEOUT)
 {
 /* if the program operation is completed, disable the OPTPG Bit */
 FLASH->CR &= CR_OPTPG_Reset;
 }
 }
 /* Return the Option Byte Data Program Status */
 return status;
}
‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍‍
FLASH_Status FLASH_EraseOptionBytes(void)
{
 uint16_t rdptmp = RDP_Key;
 FLASH_Status status = FLASH_COMPLETE;
 /* Get the actual read protection Option Byte value */
 if(FLASH_GetReadOutProtectionStatus() != RESET)
 {
 rdptmp = 0x00;
 }
 /* Wait for last operation to be completed */
 status = FLASH_WaitForLastOperation(EraseTimeout);
 if(status == FLASH_COMPLETE)
 {
 /* Authorize the small information block programming */
 FLASH->OPTKEYR = FLASH_KEY1;
 FLASH->OPTKEYR = FLASH_KEY2;
 /* if the previous operation is completed, proceed to erase the option bytes */
 FLASH->CR |= CR_OPTER_Set;
 FLASH->CR |= CR_STRT_Set;
 /* Wait for last operation to be completed */
 status = FLASH_WaitForLastOperation(EraseTimeout);
 if(status == FLASH_COMPLETE)
 {
 /* if the erase operation is completed, disable the OPTER Bit */
 FLASH->CR &= CR_OPTER_Reset;
 /* Enable the Option Bytes Programming operation */
 FLASH->CR |= CR_OPTPG_Set;
 /* Restore the last read protection Option Byte value */
 OB->RDP = (uint16_t)rdptmp;
 /* Wait for last operation to be completed */
 status = FLASH_WaitForLastOperation(ProgramTimeout);
 if(status != FLASH_TIMEOUT)
 {
 /* if the program operation is completed, disable the OPTPG Bit */
 FLASH->CR &= CR_OPTPG_Reset;
 }
 }
 else
 {
 if (status != FLASH_TIMEOUT)
 {
 /* Disable the OPTPG Bit */
 FLASH->CR &= CR_OPTPG_Reset;
 }
 }
 }
 /* Return the erase status */
 return status;
}