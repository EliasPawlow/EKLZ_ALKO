
/*
 *
 */
#include <stdint.h>
#include "Defines.h"

#ifndef __FLASH_H
#define __FLASH_H


#define IAPentry 0x1FFF1FF1


enum {
  CMD_PREPARE_SECTORS = 50,
  CMD_COPY_RAM_TO_FLASH,
  CMD_ERASE_SECTORS,
  CMD_BLANK_CHECK_SECTORS,
  CMD_READ_PART_ID,
  CMD_READ_BOOT_CODE_VERSION,
  CMD_COMPARE,
  CMD_REINVOKE_ISP,
  CMD_READ_UID,
} IAP_COMMAND;

enum {
  STATUS_CMD_SUCCESS = 0,
  STATUS_INVALID_COMMAND,
  STATUS_SRC_ADDR_ERROR,
  STATUS_DST_ADDR_ERROR,
  STATUS_SRC_ADDR_NOT_MAPPED,
  STATUS_DST_ADDR_NOT_MAPPED,
  STATUS_COUNT_ERROR,
  STATUS_INVALID_SECTOR,
  STATUS_SECTOR_NOT_BLANK,
  STATUS_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
  STATUS_COMPARE_ERROR,
  STATUS_BUSY,
} IAP_STATUS;

typedef void (__thumb *IAP)(void*, void*);

extern void FlashInit(void);
extern Status FlashWrite(uint32_t count,
                    char const *buffer);
extern Status FlashErase(void);
extern Status FlashBlankCheck(void);
extern Status FlashRead(uint32_t count, char *buffer);
#endif /*__FLASH_H*/