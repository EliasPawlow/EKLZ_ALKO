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
  uint32_t  signatur;   // Сигнатура
  char      NUM[5];   // Серийный номер, получен из криптопроцессора
  uint8_t   flags;    // Байт флагов
  ALKO_Date init_date;   // Дата инициализации
  ALKO_Time init_time;   // Время инициализации
  uint32_t  last_MAC;    // Номер последнего документа
  ALKO_Date date_MAC;    // Дата регистрации последнего документа
  ALKO_Time time_MAC;    // Время регистрации последнего документа
  uint32_t  last_ACK;    // Номер последнего подтвержденного
} ALKO_Status;

ALKO_Status ALKO;         // Блок состояния ALKO

uint8_t CMD_RCV_COUNT;
unsigned char cmd_buffer[256];//буфер ответных кодов команд.


// ============================= Отправляет команду на СП из буфера rd_UART и читает ответ в wr_UART ============================================= //
Status SendRcvdCmd()
{
  uint8_t length;
  uint16_t delay = 100;
  unsigned char *p_Out = &data_wr_UART[0];
  unsigned char *p_CmdBuff = &cmd_buffer[0];
  Timer1_Start(2500);      
  
  //-----------очистка буфера от мусора(заполнение нулями)
  memset(data_wr_UART,0,255);
  //-----------очистка буфера кодов ответа команд--------///
  memset(cmd_buffer,0,255);
  
  CS_Force(0);                  //активируем SS
  if(WAIT)                      //если перепад при неактивном WAIT
  {
    CS_Force(1);                //сбрасываем SS
    while(WAIT);                //ждем перепада WAIT в активное состояние
    CS_Force(0);                //снова активируем SS
  }           
  
  //------------отправка команды
  for(length=0;length<UART_RCV_COUNT;length++)
  {
     delay = 50;
     while(delay--);
     *p_CmdBuff++ = SPY_Byte(data_rd_UART[length]);
  }
  CMD_RCV_COUNT = 0;//очищаем значение счетчика принятых байт 
  
  CS_Force(1); 
  Timer1_Stop();
  while(!WAIT);  //ждем перепада в 1

  while(WAIT);   //ждем готовности(перепада в 0) 
  Timer1_Start(2500); 
  CS_Force(0);

  //-------------получение ответа
  SPY_Byte(0);//отправка лишнего байта чтобы избавиться от мусора(остатка последней команды)
  for(length=1;length<255;length++)
  {
     *p_Out++ = SPY_Byte(length);
     CMD_RCV_COUNT++; //принят +1 байт
     delay = 50;
     while(delay--);
    // if(WAIT) break;    //если придет WAIT==1, значит пора прекратить прием данных
  }
  CS_Force(1);

  return SUCCESS;
}


/*================== Проверка ФПО =====================================*/
void FPO_check (void) {

      muza_stat = 0;
      while (!(muza_stat & FPO)) { // В итоге бесконечно ждем, пока не появится флаг FPO
        while (!MUZA_Status());    // Ждем готовности криптопроцессора, читаем флаги
      }                           // Выход, когда FPO установлен
      while (! MUZA_FPO());     // Ждем ФПО
      while (muza_stat & FPO)  {    // Ждем готовности, читаем флаги
        while (!MUZA_Status());
      }
}