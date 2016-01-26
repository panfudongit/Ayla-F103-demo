 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "func.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size */
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)


#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08018000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0801C000)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  
uint32_t EraseCounter = 0x00, Address = 0x00;
// uint32_t Data = 0x12345678;
__IO uint32_t NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

/* Private function prototypes -----------------------------------------------*/   
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void WirteFlash(int addoff, uint32_t *data, int wordn)
{
	uint32_t *Data = data;
	int i = 0;
	
  FLASH_UnlockBank1();
	
  /* Define the number of page to be erased */
  NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
  }
  

	
//	FLASH_ProgramHalfWord(Address, (uint16_t)Data);

//   while((Address < BANK1_WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
//   {
//     FLASHStatus = FLASH_ProgramWord(Address, Data);
//     Address = Address + 4;
//   }
	
	/* Program Flash Bank1 */
  Address = BANK1_WRITE_START_ADDR + addoff;
	for(i = 0; i < wordn; i++)
	{
			FLASHStatus = FLASH_ProgramWord(Address, *(Data + i));
			Address = Address + 4;
	}

  FLASH_LockBank1();
  
//   Address = BANK1_WRITE_START_ADDR;

//   while((Address < BANK1_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
//   {
//     if((*(__IO uint32_t*) Address) != Data)
//     {
//       MemoryProgramStatus = FAILED;
//     }
//     Address += 4;
//   }
//   
//   while (1)
//   {
//   }
}

uint32_t ReadFlash(int addoff)
{
	uint32_t data;

  Address = BANK1_WRITE_START_ADDR;
	
	data = *(__IO uint32_t*) (Address + addoff);
	
	return data;
}

