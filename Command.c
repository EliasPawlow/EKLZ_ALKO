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


// ============================= Отправляет команду на СП из буфера rd_UART и читает ответ в wr_UART ============================================= //
Status SendRcvdCmd()
{
  uint8_t length;
  uint16_t delay = 100;
  unsigned char *p_Out = &data_wr_UART[0];
  Timer1_Start(2500);      
  
  //-----------очистка буфера от мусора(заполнение нулями)
  memset(data_wr_UART,0,255);
  
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
     SPY_Byte(data_rd_UART[length]);
  }
    
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
     delay = 50;
     while(delay--);
     if(WAIT) break;    //если придет WAIT==1, значит пора прекратить прием данных
  }
  CS_Force(1);

  return SUCCESS;
}



void command(void) {
  uint8_t   temp;
  uint32_t  addr;            // Адрес во флеш памяти
  uint32_t  len, count;      // Длина передаваемых данных
  unsigned char      *in;
  unsigned char      *out;
  uint32_t array_of_stat[256];
  uint32_t *p_array_of_stat;
  
  p_array_of_stat = &array_of_stat[0];
  //----------DBG---------------------------------------------------------------//
  
  //TODO после отсылки команды сопроцессору, необходимо при получении данных отправлять 0х5 вместо 0х0
  
  uint8_t _dbg_ = 0xff;
  uint8_t _tx_count_;
  uint8_t _tx_done_ = 0;
  uint8_t _rcvd_byte_ = 0;
  uint32_t __wSigValue;
  uint8_t _count_ = 0;
  uint32_t _delay_ = 10000;
  uint8_t wait_value;
  
  if(_dbg_==0xff)
  {
    
    wait_value = WAIT;
    in = &data_rd_UART[0];
    out = &data_wr_UART[0];
    //transmit data//
    //W_Wait()

      CS_Force(0);
      _tx_count_ = 3 + *(in+1);                    
    for(_tx_done_=0;_tx_done_<_tx_count_;_tx_done_++)
    {
      //*p_array_of_stat++ = GPIO_ReadValue(WAIT_PORT);
      /**out++ = */SPY_Byte(*in++);
      if (!WIN()) *out++ = 0xaa;
    }
    wait_value = WAIT;
    CS_Force(1);
    wait_value = WAIT;
    Timer1_Stop();
    //*p_array_of_stat++ = GPIO_ReadValue(WAIT_PORT);
     //while(R_PORT0(WAIT_PIN)) {/*waiting*/}
     CS_Force(0);
     wait_value = WAIT;
    //while(_delay_--);
     Timer1_Start(1000);   
    for(_tx_done_=0;_tx_done_<240;_tx_done_++)
    {
      //*p_array_of_stat++ = R_PORT0(WAIT_PIN);
      *out++ = SPY_Byte(_tx_done_);                       
       if (!WIN()) *out++ = 0x0a;
    }
      Timer1_Stop();
      CS_Force(1);
      return;
  }
  
 
  //-----------DBG--------------------------------------------------------------//

  in = &data_rd_UART[0];
  out = &data_wr_UART[0];

  data_wr_UART[0]=0x02;
  data_wr_UART[1]=0x01;     // По дефолту будем считать что ошибок нет
  data_wr_UART[2]=RET_OK;   // И прогнозируем успешное завершение команды
  data_wr_UART[3]=0x01;
  len_msg_in = len_msg_in;
 
  if (MUZA_TransmitMSG( len_msg_in, in)) 
  { 
   if (MUZA_ReceiveMSG( len_msg_out, out)) 
   {
   } else 
      {
        if (len_msg_out==0) 
        {
            data_wr_UART[1]=0x01; 
            data_wr_UART[2]=ERR_NO_ANSWER;
            ALKO.flags &= (~ERR); // Сообщим об ошибке и сбросим флаг ERR
        }
      } 
  } else 
  {
        if (len_msg_out==0) 
        {
            data_wr_UART[1]=0x01; 
            data_wr_UART[2]=ERR_NO_ANSWER;
            ALKO.flags &= (~ERR); // Сообщим об ошибке и сбросим флаг ERR
        }
  }
 len_msg_in = len_msg_in;
}

//====================================

void command_old(void) {
  uint8_t   temp;
  uint32_t  addr;            // Адрес во флеш памяти
  uint32_t  len, count;      // Длина передаваемых данных
  unsigned char      *in;
  unsigned char      *out;

//  len = data_rd_UART[1]/* - 1*/;
  in = &data_rd_UART[1];
  out = &data_wr_UART[0];

  data_wr_UART[0]=0x02;
  data_wr_UART[1]=0x01;     // По дефолту будем считать что ошибок нет
  data_wr_UART[2]=RET_OK;   // И прогнозируем успешное завершение команды
  data_wr_UART[3]=0x01;
   switch (data_rd_UART[2] & 0x7F) {              // 

//-----------------------------------------------------------------------------------
 /*              case      0x01:    //было старое 0x51:                        // Активизация MUZA
                   if (len < 5) {
                     ALKO.flags |= ERR;
                   } else {
                   // TODO Проверить флаги, запомнить параметры активизации
                    if (!MUZA_Act(len, &data_rd_UART[2])) {
                       ALKO.flags |= CRYPT;
                     } else {
                       if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                       } else {
                         data_wr_UART[2]=ERR_STAT; // Если уже инициализировано, то ошибка
                       }
                     }
                   }
                 break;
//-----------------------------------------------------------------------------------
               case     0x02:   //было старое 0x5A:          // Закрытие архива
                 if (len < 5) {
                    ALKO.flags |= ERR;
                 } else {
                 // TODO Проверить флаги
                   if (!MUZA_Close(len, &data_rd_UART[2])) {
                     ALKO.flags |= CRYPT;
                   } else {
                      if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                      } else {
                        data_wr_UART[2]=ERR_STAT; // Если уже инициализировано, то ошибка
                      }
                   }
                 }
            
                break;
//-----------------------------------------------------------------------------------
               case     0x03:   //было старое 0x58:   //MUZA_IO_Control Тест "Входной контроль"MUZA
              
                 if (!MUZA_IO_Control(100)) 
                 {
                   ALKO.flags |= CRYPT;
                 } else {
                   data_wr_UART[1] += 1;         // Один байт состояния
                   data_wr_UART[3] = muza_stat;
                 }                 
               break; 
//-----------------------------------------------------------------------------------
               case     0x04:   //не было               // MUZA_Ver_Software    Запрос версии СП 
                 // НАДО НАПИСАТЬ, входных нет, выходные - 21 байт 
                if (len < 2) {
                   ALKO.flags |= ERR;
                 } else {
                  data_wr_UART[1] += 0x15;      // Длина блока данных 21 байт
                  if (!MUZA_Ver_Software(0x15,out)) ALKO.flags |= CRYPT;
                 } 
    
                 break;
*/
//-----------------------------------------------------------------------------------
               case     0x05:   //было старое 0x57:          // Запрос состояния MUZA
                 MUZA_Status(100);
                    if (len_msg_out==0) ALKO.flags |= ERR;                
                 
               break;
 //-----------------------------------------------------------------------------------
/*               case     0x06:   //не было      //MUZA_Full_Status Запрос состояния СП
              // НАДО НАПИСАТЬ, входных нет, выходные - сколько дадут 
                 break;
*/              
//-----------------------------------------------------------------------------------
               case     0x07:    //было старое 0x64:     // Запрос регистрационного номера
                 {
                      //входных нет, выходные - Reg_Nom[0…4]
                      MUZA_num(100);
                      MUZA_num(100);
                    if (len_msg_out==0) ALKO.flags |= ERR;
                 }
               break;

//-----------------------------------------------------------------------------------
 /*              case     0x08://было старое 0x70:            // Регистрация документа (события)
                 if (len < 5) {
                    ALKO.flags |= ERR;
                 } else {
                 // TODO Проверить флаги
                   if (!MUZA_Save(len, &data_rd_UART[2])) {
                     ALKO.flags |= CRYPT;
                   } else {
                      if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                      } else {
                        data_wr_UART[2]=ERR_STAT; // Если запись не прошла, то ошибка
                      }
                   }
                 }
                break;
//-----------------------------------------------------------------------------------
               case     0x09:   //было старое 0x68:       // Ввод подтверждения о доставке
                 if (len < 5) {
                   ALKO.flags |= ERR;
                 } else {
                   if (!MUZA_check(in,out)) {
                     ALKO.flags |= CRYPT;
                   }else {
                    if (MAC.muza_mac32) {
                      data_wr_UART[1] += 3;
                      *out++ = MAC.muza_mac[0];
                      *out++ = MAC.muza_mac[1];
                      *out++ = MAC.muza_mac[2];
                     } else {
                       data_wr_UART[2]=ERR_FORMAT; // Если уже инициализировано, то ошибка
                     }
                   }
                 }
                 
                break;
//-----------------------------------------------------------------------------------
               case     0x0A:           //было старое 0x71:       // Запрос документа
                 if (len < 3) {
                   ALKO.flags |= ERR;
                 } else {
                   MAC.muza_mac[0] = *in++;
                   MAC.muza_mac[1] = *in++;
                   MAC.muza_mac[2] = *in++;
                   if (!MUZA_Doc(&temp, out)) 
                   {
                     ALKO.flags |=CRYPT;
                   } else {
                     if(temp) {
                        data_wr_UART[1] += temp;
                     } else {
                       data_wr_UART[2]=ERR_STAT;
                     }
                   }
                 }
                break;                        // события 
//-----------------------------------------------------------------------------------
               case     0x0B:   //не было      //MUZA_Integrity_Software Тест Целостности ПО
              // НАДО НАПИСАТЬ, входные-  IV[0…7], выходные - HASH_SP[0…3]||HASH_CP[0…3] 
                 break;
 //-----------------------------------------------------------------------------------
               case     0x0C:   //не было      //MUZA_READ_BUF Чтение буфера СП
              // НАДО НАПИСАТЬ, входных нет, выходные - сколько дадут 
                 break;
 //-----------------------------------------------------------------------------------
               case     0x0D:   //не было      // Отправка ПО КП в Запрос состояния СП
              // НАДО НАПИСАТЬ, входных нет, выходные - сколько дадут 
                 break;
 
//-----------------------------------------------------------------------------------
               case    0x0E:    //было старое 0x53:            // Запрос дампа архива
                 if (len < 4) {
                   ALKO.flags |= ERR;
                 } else {
                  data_wr_UART[1] += 0x80;      // Длина блока данных 128 байт
                  if (!MUZA_Dump(0x80,in,out)) ALKO.flags |= CRYPT;
                 }
                break;
//-----------------------------------------------------------------------------------
  case     0x0F:       //было старое 0x76:                       // 
                    MUZA_erase();
                    if (!MUZA_Test(10)) {
                      ALKO.flags |= CRYPT;
                      break;
                    }
                    ALKO.flags |= muza_stat;
//                    if (muza_stat != 0xC0) {
//                      ALKO.flags |= CRYPT;
//                    } else ALKO.flags = 0xC0;
                    
                break;
*/         
//-----------------------------------------------------------------------------------
             default:       // Неизвестная команда
               {
                ALKO.flags |= ERR;
               }
  }
  
  if (ALKO.flags & ERR) {
                  data_wr_UART[1]=0x01; 
                  data_wr_UART[2]=ERR_FORMAT;
                  ALKO.flags &= (~ERR); // Сообщим об ошибке и сбросим флаг ERR
                  return;
  }
  if (ALKO.flags & CRYPT) {   // Сообщим о фатальной ошибке
    data_wr_UART[1]=0x01;
    data_wr_UART[2]=ERR_KRYPTO;
    return;
  }
}



/*================== Проверка ФПО =====================================*/
void FPO_check (void) {

      muza_stat = 0;
      while (!(muza_stat & FPO)) { // В итоге бесконечно ждем, пока не появится флаг FPO
        while (!MUZA_Status(10));    // Ждем готовности криптопроцессора, читаем флаги
      }                           // Выход, когда FPO установлен
      while (! MUZA_FPO(10000));     // Ждем ФПО
      while (muza_stat & FPO)  {    // Ждем готовности, читаем флаги
        while (!MUZA_Status(10));
      }
}