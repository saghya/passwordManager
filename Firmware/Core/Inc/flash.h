#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash.h"
#include "stm32l4xx_hal_flash_ex.h"

#define NUM_OF_PAGES    (64 / 2)
#define FIRST_PAGE_ADDR (0x8000000)

void     Flash_Read_Data(uint32_t StartPageAddress, uint64_t *RxBuf,
                         uint16_t numberofdoublewords);
uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint64_t *Data,
                          uint16_t numberofdoublewords);

#endif // __FLASH_H

