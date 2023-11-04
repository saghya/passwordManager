#include "flash.h"
#include "stm32l4xx_hal_flash.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static inline uint32_t getPage(uint32_t address)
{
    return (address - FIRST_PAGE_ADDR) / FLASH_PAGE_SIZE;
}

uint32_t Flash_Write_Page(uint32_t Address, uint64_t *Data,
                          uint16_t numberofdoublewords)
{
    uint16_t numofbytes        = numberofdoublewords * sizeof(uint64_t);
    uint32_t page_shift_before = Address % FLASH_PAGE_SIZE;
    uint32_t StartPageAddress  = Address - page_shift_before;
    uint32_t EndAddress        = Address + numofbytes;
    uint32_t page_shift_after =
        FLASH_PAGE_SIZE - (EndAddress % FLASH_PAGE_SIZE);
    if (EndAddress - StartPageAddress > 2 * FLASH_PAGE_SIZE) {
        return -1;
    }
    uint8_t buff[2 * FLASH_PAGE_SIZE] = {0};
    memcpy(buff, (uint8_t *)StartPageAddress, page_shift_before);
    memcpy(buff + page_shift_before, (uint8_t *)Data, numofbytes);
    memcpy(buff + page_shift_before + numofbytes, (uint8_t *)EndAddress,
           page_shift_after);

    return Flash_Write_Data(
        StartPageAddress, (uint64_t *)buff,
        (page_shift_before + numofbytes + page_shift_after) / 8);
}

uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint64_t *Data,
                          uint16_t numberofdoublewords)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t                      PAGEError;

    /* Unlock the Flash to enable the flash control register access
     * *************/
    HAL_FLASH_Unlock();

    /* Erase the user Flash area*/

    uint32_t StartPage     = getPage(StartPageAddress);
    uint32_t EndPageAdress = StartPageAddress + numberofdoublewords * 8;
    uint32_t EndPage       = getPage(EndPageAdress);

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page      = StartPage;
    EraseInitStruct.NbPages   = EndPage - StartPage + 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        /*Error occurred while page erase.*/
        return HAL_FLASH_GetError();
    }

    /* Program the user Flash area doubleword by doubleword*/

    for (uint32_t i = 0; i < numberofdoublewords; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress,
                              Data[i]) == HAL_OK) {
            StartPageAddress += 8;
        } else {
            /* Error occurred while writing data in Flash memory*/
            return HAL_FLASH_GetError();
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return 0;
}

void Flash_Read_Data(uint32_t StartPageAddress, uint64_t *RxBuf,
                     uint16_t numberofdoublewords)
{
    while (1) {
        *RxBuf = *(__IO uint64_t *)StartPageAddress;
        StartPageAddress += 8;
        RxBuf++;
        if (!(numberofdoublewords--))
            break;
    }
}

