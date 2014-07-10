/**
 * @file		template.c
 * @purpose		A simple template for GNU example
 * @version		x.x
 * @date		dd. mm. yy
 * @author		xxxx
 *----------------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
/* Include files */
/* <<add include files here>> */

#include "Defines.h"
#include "modem.h"
#include "gprs.h"
#include "crypto.h"

/**
 * @brief Main program body
 */
CAN_Status Channel; // ����������, ������������ ���������, �� �������� �������� 
                    
//======== ������������ ������ ������ data_wr_UART ===================
void Write_Buffer(void) {    // ����� ������ � ������� ���� � �����
  uint32_t   len;              // ����� ��������� �� 2-�� ����� ������
  uint32_t  count;            // ����� ����� � ������ ��� ������
  uint32_t  tmp;
  
  len = 9;
  count = 0;
      while (len) {
      UART_SendByte(UART0, data_wr_UART[count]);
      tmp = 20000;
      while(tmp--);
      count++;
      len--;
    }
 
}


//=============================================================================
int c_entry(void)
{
  
  Init_Uart_Default(); //�������� ��������� ���������� ����������� UART
  Init_Timer0();       // ������ ����� ������������ �������� � ����������
  Init_Timer1();
  Init_GPIO();         // ������������� ������
  Wait(1000);          // ����� 1 �������
  Init_Uart0();        // ������������� � ��������� UART0
  
  
  time_out = 50;
  Timer_Init();
  Init_SPI();
 
  Int_UART0_Enable();  // ���������� ���������� �� UART0
  
  Init_SPI();
  FPO_check();   // ���������, �� ���� �� ������ FPO
  UART_refresh();
  
  while(1) 
  {
    if(UART_DONE==0xff)
    {
     SendRcvdCmd();             //���������� � ������ ���, ��� ������ �� UART
     UART_refresh();            //���������� ����� UART
     Write_Buffer();
    }
  }
return 1;
}



/* Support required entry point for other toolchain */
int main (void)
{
       
	return c_entry();
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

