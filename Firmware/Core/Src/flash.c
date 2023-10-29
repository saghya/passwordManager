#include "flash.h"

static inline uint32_t getPage(uint32_t address)
{
    return (address - FIRST_PAGE_ADDR) / FLASH_PAGE_SIZE;
}

uint32_t Flash_Write_Data(uint32_t StartPageAddress, uint64_t *Data, uint16_t numberofdoublewords)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t                      PAGEError;
    int                           sofar = 0;

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

    while (sofar < numberofdoublewords) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, Data[sofar]) == HAL_OK) {
            StartPageAddress += 8; // use StartPageAddress += 2 for half word and 8 for double word
            sofar++;
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

void Flash_Read_Data(uint32_t StartPageAddress, uint64_t *RxBuf, uint16_t numberofdoublewords)
{
    while (1) {
        *RxBuf = *(__IO uint64_t *)StartPageAddress;
        StartPageAddress += 8;
        RxBuf++;
        if (!(numberofdoublewords--))
            break;
    }
}

