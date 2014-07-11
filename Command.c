#include <string.h>
#include "Defines.h"
#include "crypto.h"
#include <stdlib.h>

#define SIGN  0xAA5533CC

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t day;
} ALKO_Date;

typedef struct {
  uint8_t hour;
  uint8_t min;
} ALKO_Time;

typedef struct {
  uint32_t  signatur;   // ���������
  char      NUM[5];   // �������� �����, ������� �� ����������������
  uint8_t   flags;    // ���� ������
  ALKO_Date init_date;   // ���� �������������
  ALKO_Time init_time;   // ����� �������������
  uint32_t  last_MAC;    // ����� ���������� ���������
  ALKO_Date date_MAC;    // ���� ����������� ���������� ���������
  ALKO_Time time_MAC;    // ����� ����������� ���������� ���������
  uint32_t  last_ACK;    // ����� ���������� ���������������
} ALKO_Status;

ALKO_Status ALKO;         // ���� ��������� ALKO

uint8_t CMD_RCV_COUNT;
unsigned char cmd_buffer[256];//����� �������� ����� ������.


// ============================= ���������� ������� �� �� �� ������ rd_UART � ������ ����� � wr_UART ============================================= //
Status SendRcvdCmd()
{
  uint8_t length;
  uint16_t delay = 100;
  unsigned char *p_Out = &data_wr_UART[0];
  unsigned char *p_CmdBuff = &cmd_buffer[0];
  Timer1_Start(2500);      
  
  //-----------������� ������ �� ������(���������� ������)
  memset(data_wr_UART,0,255);
  //-----------������� ������ ����� ������ ������--------///
  memset(cmd_buffer,0,255);
  
  CS_Force(0);                  //���������� SS
  if(WAIT)                      //���� ������� ��� ���������� WAIT
  {
    CS_Force(1);                //���������� SS
    while(WAIT);                //���� �������� WAIT � �������� ���������
    CS_Force(0);                //����� ���������� SS
  }           
  
  //------------�������� �������
  for(length=0;length<UART_RCV_COUNT;length++)
  {
     delay = 50;
     while(delay--);
     *p_CmdBuff++ = SPY_Byte(data_rd_UART[length]);
  }
  CMD_RCV_COUNT = 0;//������� �������� �������� �������� ���� 
  
  CS_Force(1); 
  Timer1_Stop();
  while(!WAIT);  //���� �������� � 1

  while(WAIT);   //���� ����������(�������� � 0) 
  Timer1_Start(2500); 
  CS_Force(0);

  //-------------��������� ������
  SPY_Byte(0);//�������� ������� ����� ����� ���������� �� ������(������� ��������� �������)
  for(length=1;length<255;length++)
  {
     *p_Out++ = SPY_Byte(length);
     CMD_RCV_COUNT++; //������ +1 ����
     delay = 50;
     while(delay--);
    // if(WAIT) break;    //���� ������ WAIT==1, ������ ���� ���������� ����� ������
  }
  CS_Force(1);

  return SUCCESS;
}


/*================== �������� ��� =====================================*/
void FPO_check (void) {

      muza_stat = 0;
      while (!(muza_stat & FPO)) { // � ����� ���������� ����, ���� �� �������� ���� FPO
        while (!MUZA_Status());    // ���� ���������� ����������������, ������ �����
      }                           // �����, ����� FPO ����������
      while (! MUZA_FPO());     // ���� ���
      while (muza_stat & FPO)  {    // ���� ����������, ������ �����
        while (!MUZA_Status());
      }
}