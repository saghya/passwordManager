#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>

#define NUM_OF_PAGES    ((uint32_t)64)
#define FIRST_PAGE_ADDR ((uint32_t)0x8000000)

void     Flash_Read_Data(uint32_t StartPageAddress, uint64_t *RxBuf, uint16_t numberofdoublewords);
uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint64_t *Data, uint16_t numberofdoublewords);
uint32_t Flash_Write_Page(uint32_t Address, uint64_t *Data, uint16_t numberofdoublewords);

#endif // __FLASH_H

