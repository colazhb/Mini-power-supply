#include "mcu_flash.h"
#include "board_pin_def.h"

// 写到最后1K的位置
#define FLASH_ROM_SIZE 32  // 32K
#define FLASH_PAGE_SIZE  1024
// 计算写的位置
#define FLASH_ROM_WRITE_ADDR  (FLASH_BASE + FLASH_ROM_SIZE * 1024 - FLASH_PAGE_SIZE)
#define FMC_WRITE_END_ADDR    (FLASH_ROM_WRITE_ADDR + FLASH_PAGE_SIZE)

uint8_t flash_data[FLASH_PAGE_SIZE] = {0};

void flash_erase(uint32_t addr)
{
     fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    
    fmc_page_erase(addr);
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    fmc_lock();
};
void flash_program(uint32_t address , uint32_t* data)
{
   fmc_unlock();

    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_word_program(address, data);
        address += 4U;
        data++;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
};
void mcu_flash_erase()
{
    flash_erase(FLASH_ROM_WRITE_ADDR);
};
void mcu_flash_read(uint8_t *data , uint16_t len)
{
    uint32_t read_data = 0;
    uint32_t* addr = (uint32_t *)FLASH_ROM_WRITE_ADDR;
    int j = 0;
    for (int i = 0; i < len; i++)
    {
       read_data = *addr;
       data[j] = read_data & 0xff;
       data[j+1] = (read_data >> 8) & 0xff;
       data[j+2] = (read_data >> 16) & 0xff;
       data[j+3] = (read_data >> 24) & 0xff;
       addr++;
         j += 4;

    }
};


void mcu_flash_write(uint8_t *data , uint16_t len)
{
    uint32_t addr = FLASH_ROM_WRITE_ADDR;
    for (int i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        flash_data[i] = *(uint8_t *)addr;
        addr++;
    }
    for (int i = 0; i < len; i++)
    {
        flash_data[i] = data[i];
    }
    flash_erase(FLASH_ROM_WRITE_ADDR);
    flash_program(FLASH_ROM_WRITE_ADDR, (uint32_t *)flash_data);

};