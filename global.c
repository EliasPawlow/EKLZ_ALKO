#include "Defines.h"
#include "lpc17xx_uart.h"

//==================== Переменные, связанные с UART0 - касса ==================
unsigned char data_rd_UART[MSG_SIZE_RD];
unsigned char data_wr_UART[MSG_SIZE_WR];
//==================== Переменные, связанные со SPI - криптопроцессор =========
//unsigned char data_rd_SPI[MSG_SIZE_RD_SPI];
//unsigned char data_wr_SPI[MSG_SIZE_WR_SPI];
//==================== Переменные, связанные с UART1 - GSM ====================
//unsigned char data_rd_GSM[MSG_SIZE_RD_GSM];
//unsigned char data_wr_GSM[MSG_SIZE_WR_GSM];
//=============================================================================