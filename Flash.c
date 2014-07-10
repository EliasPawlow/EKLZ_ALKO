#include <string.h>
#include "Flash.h"
/** local definitions **/
//==============================================
//==============================================
//==============================================

/** default settings **/

/** external functions **/
//extern uint32_t SYS_GetFsclk(void);

/** external data **/

/** internal functions **/

static int ExecuteCommand(unsigned long* cmd, unsigned long* status);

/** public data **/

/** private data **/
const static IAP iap_entry = (IAP)IAPentry; // MCU flash firmware interface function.
// The CPU clock speed (CCLK), the default value is used if no clock option is found.
//=====================================================================================
void FlashInit(void) {
  SystemCoreClockUpdate();
}
//=====================================================================================
static int ExecuteCommand(unsigned long* cmd, unsigned long* status)
{
  int ret;

  for (;;)
  {
    __disable_irq();
    iap_entry(cmd, status);
    __enable_irq();
    ret = status[0];
    if (ret != STATUS_BUSY)
    {
      return ret;
    }
    // Try again if busy.
  }
}

//============================================================================
Status FlashWrite(uint32_t count, char const *buffer)
{
  int ret;
  unsigned int  Size ;
  unsigned long cmd[5];
  unsigned long status[3];


      if (count > 4096) return ERROR;
//======== ¬ычислим размер записываемого блока данных =======================
      if (count < 257) {
        Size = 256;
      } else {
        if (count < 513) {
          Size = 512;
        } else {
          if (count < 1025) {
            Size = 1024;
          } else {
            Size = 4096;
          }
        }
      }
//==========================================================================  
      cmd[0] = CMD_PREPARE_SECTORS;
      cmd[1] = 1;
      cmd[2] = 1;

      ret = ExecuteCommand(cmd, status);

      if (ret != STATUS_CMD_SUCCESS)
      {
        //message CMD_PREPARE_SECTORS failed.
        return ERROR;
      }

      cmd[0] = CMD_COPY_RAM_TO_FLASH;
      cmd[1] = 0x1000;
      cmd[2] = (unsigned long)buffer;
      cmd[3] = Size;
      cmd[4] = SystemCoreClock / 1000;
       ret = ExecuteCommand(cmd, status);


      if (ret != STATUS_CMD_SUCCESS)
      {
        //message CMD_COPY_RAM_TO_FLASH failed.
        return ERROR;
      }

    return SUCCESS;
}
//============================================================================
Status FlashErase(void)
{
  int ret;
  unsigned long cmd[4];
  unsigned long status[3];


    // Prepare sector for erase.
    cmd[0] = CMD_PREPARE_SECTORS;
    cmd[1] = 1;
    cmd[2] = 1;

    ret = ExecuteCommand(cmd, status);
   if (ret != STATUS_CMD_SUCCESS)
    {
      //message CMD_PREPARE_SECTORS failed.
      return ERROR;
    }

    // Erase sector.
    cmd[0] = CMD_ERASE_SECTORS;
    cmd[1] = 1;
    cmd[2] = 1;    
   // cmd[3] = SYS_GetFsclk()/1000;
    
    //cmd[3] = SYS_GetFsclk()/4;
    //cmd[3] = 96000;    //96000
    //cmd[3] = 3200;
    cmd[3] = SystemCoreClock / 1000;
    ret = ExecuteCommand(cmd, status);

    if (ret != STATUS_CMD_SUCCESS)
    {
      //message CMD_ERASE_SECTORS failed.
      return ERROR;
    }

    return SUCCESS;
}
//=============================================================================
Status FlashBlankCheck(void)
{
  int ret;
  unsigned long cmd[4];
  unsigned long status[3];

    // Prepare sector for erase.
    cmd[0] = CMD_BLANK_CHECK_SECTORS;
    cmd[1] = 1;
    cmd[2] = 1;
    cmd[3] = SystemCoreClock / 1000;


    ret = ExecuteCommand(cmd, status);

   if (ret == STATUS_SECTOR_NOT_BLANK)
    {
      /*Sector is not Blank, Invalid Sector or
        param error*/
      return ERROR;
    }

    return SUCCESS;
}
//=============================================================================
Status FlashRead(uint32_t count, char *buffer) {
  
  if (count > 4096) return ERROR;
  memcpy(buffer, (const void *)0x1000, count);
  return SUCCESS;
  
}