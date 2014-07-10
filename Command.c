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


// ============================= ���������� ������� �� �� �� ������ rd_UART � ������ ����� � wr_UART ============================================= //
Status SendRcvdCmd()
{
  uint8_t length;
  uint16_t delay = 100;
  unsigned char *p_Out = &data_wr_UART[0];
  Timer1_Start(2500);      
  
  //-----------������� ������ �� ������(���������� ������)
  memset(data_wr_UART,0,255);
  
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
     SPY_Byte(data_rd_UART[length]);
  }
    
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
     delay = 50;
     while(delay--);
     if(WAIT) break;    //���� ������ WAIT==1, ������ ���� ���������� ����� ������
  }
  CS_Force(1);

  return SUCCESS;
}



void command(void) {
  uint8_t   temp;
  uint32_t  addr;            // ����� �� ���� ������
  uint32_t  len, count;      // ����� ������������ ������
  unsigned char      *in;
  unsigned char      *out;
  uint32_t array_of_stat[256];
  uint32_t *p_array_of_stat;
  
  p_array_of_stat = &array_of_stat[0];
  //----------DBG---------------------------------------------------------------//
  
  //TODO ����� ������� ������� ������������, ���������� ��� ��������� ������ ���������� 0�5 ������ 0�0
  
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
  data_wr_UART[1]=0x01;     // �� ������� ����� ������� ��� ������ ���
  data_wr_UART[2]=RET_OK;   // � ������������ �������� ���������� �������
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
            ALKO.flags &= (~ERR); // ������� �� ������ � ������� ���� ERR
        }
      } 
  } else 
  {
        if (len_msg_out==0) 
        {
            data_wr_UART[1]=0x01; 
            data_wr_UART[2]=ERR_NO_ANSWER;
            ALKO.flags &= (~ERR); // ������� �� ������ � ������� ���� ERR
        }
  }
 len_msg_in = len_msg_in;
}

//====================================

void command_old(void) {
  uint8_t   temp;
  uint32_t  addr;            // ����� �� ���� ������
  uint32_t  len, count;      // ����� ������������ ������
  unsigned char      *in;
  unsigned char      *out;

//  len = data_rd_UART[1]/* - 1*/;
  in = &data_rd_UART[1];
  out = &data_wr_UART[0];

  data_wr_UART[0]=0x02;
  data_wr_UART[1]=0x01;     // �� ������� ����� ������� ��� ������ ���
  data_wr_UART[2]=RET_OK;   // � ������������ �������� ���������� �������
  data_wr_UART[3]=0x01;
   switch (data_rd_UART[2] & 0x7F) {              // 

//-----------------------------------------------------------------------------------
 /*              case      0x01:    //���� ������ 0x51:                        // ����������� MUZA
                   if (len < 5) {
                     ALKO.flags |= ERR;
                   } else {
                   // TODO ��������� �����, ��������� ��������� �����������
                    if (!MUZA_Act(len, &data_rd_UART[2])) {
                       ALKO.flags |= CRYPT;
                     } else {
                       if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                       } else {
                         data_wr_UART[2]=ERR_STAT; // ���� ��� ����������������, �� ������
                       }
                     }
                   }
                 break;
//-----------------------------------------------------------------------------------
               case     0x02:   //���� ������ 0x5A:          // �������� ������
                 if (len < 5) {
                    ALKO.flags |= ERR;
                 } else {
                 // TODO ��������� �����
                   if (!MUZA_Close(len, &data_rd_UART[2])) {
                     ALKO.flags |= CRYPT;
                   } else {
                      if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                      } else {
                        data_wr_UART[2]=ERR_STAT; // ���� ��� ����������������, �� ������
                      }
                   }
                 }
            
                break;
//-----------------------------------------------------------------------------------
               case     0x03:   //���� ������ 0x58:   //MUZA_IO_Control ���� "������� ��������"MUZA
              
                 if (!MUZA_IO_Control(100)) 
                 {
                   ALKO.flags |= CRYPT;
                 } else {
                   data_wr_UART[1] += 1;         // ���� ���� ���������
                   data_wr_UART[3] = muza_stat;
                 }                 
               break; 
//-----------------------------------------------------------------------------------
               case     0x04:   //�� ����               // MUZA_Ver_Software    ������ ������ �� 
                 // ���� ��������, ������� ���, �������� - 21 ���� 
                if (len < 2) {
                   ALKO.flags |= ERR;
                 } else {
                  data_wr_UART[1] += 0x15;      // ����� ����� ������ 21 ����
                  if (!MUZA_Ver_Software(0x15,out)) ALKO.flags |= CRYPT;
                 } 
    
                 break;
*/
//-----------------------------------------------------------------------------------
               case     0x05:   //���� ������ 0x57:          // ������ ��������� MUZA
                 MUZA_Status(100);
                    if (len_msg_out==0) ALKO.flags |= ERR;                
                 
               break;
 //-----------------------------------------------------------------------------------
/*               case     0x06:   //�� ����      //MUZA_Full_Status ������ ��������� ��
              // ���� ��������, ������� ���, �������� - ������� ����� 
                 break;
*/              
//-----------------------------------------------------------------------------------
               case     0x07:    //���� ������ 0x64:     // ������ ���������������� ������
                 {
                      //������� ���, �������� - Reg_Nom[0�4]
                      MUZA_num(100);
                      MUZA_num(100);
                    if (len_msg_out==0) ALKO.flags |= ERR;
                 }
               break;

//-----------------------------------------------------------------------------------
 /*              case     0x08://���� ������ 0x70:            // ����������� ��������� (�������)
                 if (len < 5) {
                    ALKO.flags |= ERR;
                 } else {
                 // TODO ��������� �����
                   if (!MUZA_Save(len, &data_rd_UART[2])) {
                     ALKO.flags |= CRYPT;
                   } else {
                      if (MAC.muza_mac32) {
                        data_wr_UART[1] += 3;
                        *out++ = MAC.muza_mac[0];
                        *out++ = MAC.muza_mac[1];
                        *out++ = MAC.muza_mac[2];
                      } else {
                        data_wr_UART[2]=ERR_STAT; // ���� ������ �� ������, �� ������
                      }
                   }
                 }
                break;
//-----------------------------------------------------------------------------------
               case     0x09:   //���� ������ 0x68:       // ���� ������������� � ��������
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
                       data_wr_UART[2]=ERR_FORMAT; // ���� ��� ����������������, �� ������
                     }
                   }
                 }
                 
                break;
//-----------------------------------------------------------------------------------
               case     0x0A:           //���� ������ 0x71:       // ������ ���������
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
                break;                        // ������� 
//-----------------------------------------------------------------------------------
               case     0x0B:   //�� ����      //MUZA_Integrity_Software ���� ����������� ��
              // ���� ��������, �������-  IV[0�7], �������� - HASH_SP[0�3]||HASH_CP[0�3] 
                 break;
 //-----------------------------------------------------------------------------------
               case     0x0C:   //�� ����      //MUZA_READ_BUF ������ ������ ��
              // ���� ��������, ������� ���, �������� - ������� ����� 
                 break;
 //-----------------------------------------------------------------------------------
               case     0x0D:   //�� ����      // �������� �� �� � ������ ��������� ��
              // ���� ��������, ������� ���, �������� - ������� ����� 
                 break;
 
//-----------------------------------------------------------------------------------
               case    0x0E:    //���� ������ 0x53:            // ������ ����� ������
                 if (len < 4) {
                   ALKO.flags |= ERR;
                 } else {
                  data_wr_UART[1] += 0x80;      // ����� ����� ������ 128 ����
                  if (!MUZA_Dump(0x80,in,out)) ALKO.flags |= CRYPT;
                 }
                break;
//-----------------------------------------------------------------------------------
  case     0x0F:       //���� ������ 0x76:                       // 
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
             default:       // ����������� �������
               {
                ALKO.flags |= ERR;
               }
  }
  
  if (ALKO.flags & ERR) {
                  data_wr_UART[1]=0x01; 
                  data_wr_UART[2]=ERR_FORMAT;
                  ALKO.flags &= (~ERR); // ������� �� ������ � ������� ���� ERR
                  return;
  }
  if (ALKO.flags & CRYPT) {   // ������� � ��������� ������
    data_wr_UART[1]=0x01;
    data_wr_UART[2]=ERR_KRYPTO;
    return;
  }
}



/*================== �������� ��� =====================================*/
void FPO_check (void) {

      muza_stat = 0;
      while (!(muza_stat & FPO)) { // � ����� ���������� ����, ���� �� �������� ���� FPO
        while (!MUZA_Status(10));    // ���� ���������� ����������������, ������ �����
      }                           // �����, ����� FPO ����������
      while (! MUZA_FPO(10000));     // ���� ���
      while (muza_stat & FPO)  {    // ���� ����������, ������ �����
        while (!MUZA_Status(10));
      }
}