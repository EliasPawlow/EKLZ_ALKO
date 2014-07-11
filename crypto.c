#include "Defines.h"
#include "crypto.h"

uint8_t muza_stat;
char    muza_buff[256];  // ����� ��� ������ � �����������������
MAC_num MAC;

uint8_t muza_len;        // ����� ������ � ������ ����������������

Status WIN (void);
// ��������� ������������ �������� WAIT ��� ������� �������
// ���� �� ���������, ������ ������������� � CS �������
Status WIN (void) {

   while(WAIT) {
      if (Timer1_Status()) {
        Timer1_Stop();
        CS_Force(1);
        return ERROR;
      }
   }
  return SUCCESS;
}

//--------------------����������� ��	MUZA_Act	0x01    D[0] = n = 17h, D[1�n] 

Status MUZA_Act(uint8_t len, char *ptr) {

  uint8_t tmp;

  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;   
 
  CS_Force(0);
  muza_stat = SPY_Byte(0x01);

  if (!WIN()) return ERROR;       SPY_Byte(len);          
  if (!WIN()) return ERROR;

  while (len) {
    if (!WIN()) return ERROR;       tmp = SPY_Byte(*ptr++);
    len--;
  }

    if (!WIN()) return ERROR;       MAC.muza_mac[0]=SPY_Byte(0xff);
    if (!WIN()) return ERROR;       MAC.muza_mac[1]=SPY_Byte(0xff);
    if (!WIN()) return ERROR;       MAC.muza_mac[2]=SPY_Byte(0xff);

  Timer1_Stop();
  CS_Force(1);
  MAC.muza_mac[3] = 0;
  return SUCCESS;   
}
  
//---------------------�������� ������ 	MUZA_Close	0x02    D[0] = n = 17h, D[1�n] 

Status MUZA_Close(uint8_t len, char *ptr) {

  

  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;  CS_Force(0);

  muza_stat = SPY_Byte(0x04);  if (!WIN()) return ERROR;

  SPY_Byte(len);          if (!WIN()) return ERROR;

  while (len) {
    if (!WIN()) return ERROR;
    SPY_Byte(*ptr++);
    len--;
  }

  if (!WIN()) return ERROR;   MAC.muza_mac[0]=SPY_Byte(0xff);   
  if (!WIN()) return ERROR;   MAC.muza_mac[1]=SPY_Byte(0xff); 
  if (!WIN()) return ERROR;   MAC.muza_mac[2]=SPY_Byte(0xff);
  Timer1_Stop();
  CS_Force(1);
  MAC.muza_mac[3] = 0;
  return SUCCESS;   
}

//---------------------���� �������� ���������	 MUZA_IO_Control	0x03    ���
Status MUZA_IO_Control(uint32_t delay) {

  Timer1_Start(delay);    // ����� ����� ���������� delay/10 ���.

  if (!WIN()) return ERROR;

  CS_Force(0);
  
  muza_stat = SPY_Byte(0x03);   //
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;   
}
//---------------------MUZA_Test- ���� ������
// ������� ��������� ������������� ��� ������������
Status MUZA_Test(uint32_t delay) {
   Timer1_Start(delay);    // ����� ����� ���������� delay/10 ���.
   
   if (!WIN()) return ERROR;
   
   CS_Force(0);
   muza_stat= SPY_Byte(0x03); //����� ������� �������������         ��� �������???
   CS_Force(1);
   
   if (!WIN()) return ERROR;

   CS_Force(0);
   muza_stat = SPY_Byte(0x05); // ������ ������� ����� �������������
   CS_Force(1);
   
   Timer1_Stop();
//    if (muza_stat == 0xC0) 
      return SUCCESS;
    return ERROR;
}

//---------------------������ ������ �� ��	MUZA_Ver_Software	0x04    ���
Status MUZA_Ver_Software(uint8_t len, char *out) {
  Timer1_Start(1000);           // ��������� ������� 10 ���.
  if (!WIN()) return ERROR;
  CS_Force(0);
  muza_stat = SPY_Byte(0x04);
    while (len) {
      if (!WIN()) return ERROR;
      *out++=SPY_Byte(0x00);
      len--;
    }
    CS_Force(1);
    Timer1_Stop();
    return SUCCESS;    
}


//---------------------������ ����� ���������	MUZA_Status	        0x05    ���
// ������ ��������� ����������������. ���� ERROR, �� ������, ����� muza_stat 

Status MUZA_Status()
{
  //���������� ������
  UART_RCV_COUNT = 4;
  data_rd_UART[0] = 0x2;
  data_rd_UART[1] = 0x1;
  data_rd_UART[2] = 0x5;
  data_rd_UART[3] = 0x4;
  //�������� ������
  if(SendRcvdCmd())
  {
    muza_stat = cmd_buffer[0];
    return SUCCESS;
  }
  return ERROR;
}



//---------------------������ ���������������� ������ 	MUZA_num	0x07    Nom_Excise[0�4]

Status MUZA_num(uint32_t delay) { // ������ ��������� ������
  uint32_t  len = 254;
  
   Timer1_Start(delay);         // ����� ����� ���������� delay/10 ���.
  
   if (!WIN()) return ERROR;
   CS_Force(0);

   muza_stat = SPY_Byte(0x02); //����� ������� ������ ������ { 0x02 0x01 0x07 0x06 }
//    if (!WIN()) return ERROR;
  SPY_Byte(0x01);
//    if (!WIN()) return ERROR;
  SPY_Byte(0x07);              
//    if (!WIN()) return ERROR;
  SPY_Byte(0x06);
 //   if (!WIN()) return ERROR;
     CS_Force(1);       // ������ ������ CS � ����� ������� ������� �� ����� 

 len_msg_out=0;
  
        if (!WIN()) return ERROR; 
     CS_Force(0);
  while (len) {
       if (!WIN()) return ERROR;  
     data_wr_UART[len_msg_out]=SPY_Byte(0x00);
        if (!WIN()) return ERROR;
      len_msg_out++;
      len--;
    }
    CS_Force(1);
    Timer1_Stop();
    return SUCCESS;  
}

//---------------------����������� ���������	MUZA_Save	0x08    D[0] = n,D[1�n] (20 ? n ? 240)

Status MUZA_Save(uint8_t len, char *ptr) {

  
  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;

  CS_Force(0);    
  muza_stat = SPY_Byte(0x08); 
  if (!WIN()) return ERROR;   SPY_Byte(len); 
  if (!WIN()) return ERROR;

  while (len) {
    if (!WIN()) return ERROR;
    SPY_Byte(*ptr++);
    len--;
  }
//  while(!( WAIT ));

  if (!WIN()) return ERROR;  MAC.muza_mac[0]=SPY_Byte(0xff);
  if (!WIN()) return ERROR;  MAC.muza_mac[1]=SPY_Byte(0xff);
  if (!WIN()) return ERROR;  MAC.muza_mac[2]=SPY_Byte(0xff);
  Timer1_Stop();
  CS_Force(1);
  MAC.muza_mac[3] = 0;
  return SUCCESS; 
}

  
//---------------------����  �������������	MUZA_check	0x09    Nom_MAC[0�2], Conf_MAC[0,1]

Status MUZA_check(char *in, char *out) {

  Timer1_Start(1000);           // ��������� ������� 10 ���.
  if (!WIN()) return ERROR;
  CS_Force(0);
  muza_stat = SPY_Byte(0x09);
  SPY_Byte(*in++);      // ��� ����� ������
  SPY_Byte(*in++);
  SPY_Byte(*in++);
  SPY_Byte(*in++);      //  � ��� ����� ����  �������������
  SPY_Byte(*in++);
  while(!( WAIT ));
  if (!WIN()) return ERROR;
     *out++=SPY_Byte(0x00);
     *out++=SPY_Byte(0x00);
     *out++=SPY_Byte(0x00);
     Timer1_Stop();
     CS_Force(1);
     return SUCCESS;
}


//---------------------������ ��������� �� ������ MAC	MAC.muza_mac ???        0x0A	Nom_MAC[0�2]	
//---------------------������� �������� ������������� ������� ���������
//---------------------� ����������� ���������������� ������� MAC

Status MUZA_Doc(uint8_t *count, unsigned char *ptr) {
  
  uint8_t len;
 
  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;

  CS_Force(0);
  muza_stat = SPY_Byte(0x0A);
  SPY_Byte(MAC.muza_mac[0]);
  SPY_Byte(MAC.muza_mac[1]);
  SPY_Byte(MAC.muza_mac[2]);
  while(!( WAIT ));
  if (!WIN()) return ERROR;
  len = SPY_Byte(0x00);
  *count = len;                
  while (len) {
    if (!WIN()) return ERROR;
    *ptr++=SPY_Byte(0x00);
    len--;
  }
  Timer1_Stop();
  CS_Force(1);
  return SUCCESS;
}

//------------------------------������� �������� ��� � ���������������--------------------//
Status MUZA_FPO()
{
  uint8_t LRC;                  //����������� �����. ����������� � ��������
  uint8_t bToSend;              //������� ������������ ���� � ��
  uint32_t dwCurrent;           //����� ������� �������� ����� ��������
  uint32_t dwCount = 0x2000;    //���������� �������� ����� �������� = 8192(0�2000)
  uint8_t bSmallIterator;
  
  uint8_t *p_InBuffer;/* = &data_rd_UART[0];*/
  
  //��������� ���� 8192 ����(0�2000)
  for(dwCurrent=0; dwCurrent<dwCount; dwCurrent++)
  {
    p_InBuffer = &data_rd_UART[0];
    
    *p_InBuffer++ = 0x02;       //stx
    *p_InBuffer++ = 0x43;       //����� = 67
    *p_InBuffer++ = 0x0d;       //�������
    
    LRC = 0x4e;                 // LRC = 0xe = 0x43^0x0d
    bToSend = dwCurrent & 0xff; //������� ������������ ����
    LRC ^= bToSend;
    *p_InBuffer++ = bToSend;
    
    bToSend = (dwCurrent>>8) & 0xff;
    LRC ^= bToSend;
    *p_InBuffer++ = bToSend;
    
    //���������� � �������� ������ �������
    for(bSmallIterator=0; bSmallIterator<64; bSmallIterator++)
    {
      bToSend = bSmallIterator & 0xff;
      LRC ^= bToSend;
      *p_InBuffer++ = bToSend;
    }
    //���������� � �������� LRC
    *p_InBuffer++ = LRC;
    
    //������� �������� � ��������� ����������
    UART_RCV_COUNT = 70;
    SendRcvdCmd();
 
  }
  
  //������ ����� �������
  p_InBuffer = &data_rd_UART[0];
  
  *p_InBuffer++ = 0x02;
  *p_InBuffer++ = 0x03;
  *p_InBuffer++ = 0x0D;
  LRC = 0x0E;
  
  bToSend = dwCount & 0xff;
  LRC ^= bToSend;
  *p_InBuffer++ = bToSend;
  
  bToSend = (dwCount>>8) & 0xff;
  LRC ^= bToSend;
  *p_InBuffer++ = bToSend;
  *p_InBuffer++ = LRC;
  //�������� � ��������� ���������� 2 ����� �������
  UART_RCV_COUNT = 6;
  SendRcvdCmd();
  
  return SUCCESS;

}


//---------------------������ ������	MUZA_Dump	0x0E	�[3..0]

Status MUZA_Dump(uint32_t len, char *addr, char *ptr) {
  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;

  if (*addr++) {
    if(!MUZA_two()) return ERROR;
  } else {
    if(!MUZA_one()) return ERROR;
  }
  CS_Force(0);
  muza_stat = SPY_Byte(0x0E);
  SPY_Byte(*addr++);
  SPY_Byte(*addr++);
  SPY_Byte(*addr++);
  while (len) {
    if (!WIN()) return ERROR;
    *ptr++=SPY_Byte(0x00);
    len--;
  }
  Timer1_Stop();
  CS_Force(1);
  return SUCCESS;
}

//---------------------�������� ������	MUZA_erase	0x0F	���

// �������� ������. ���� ����� �������� ��� ����� � ���������� ��������� 400, �� ������
Status MUZA_erase(void) {
  Timer1_Start(4000); // 400 ������ �� �������� ���� ������

  if (!WIN()) return ERROR;

  CS_Force(0);
  muza_stat = SPY_Byte   (0x0F);               //(0x06);
  CS_Force(1);

  if (!WIN()) return ERROR;

  Timer1_Stop();
  return SUCCESS;
}
//---------------------
//---------------------



Status MUZA_one(void) {
     Timer1_Start(10);    // ����� ����� ���������� 1 ���.

     if (!WIN()) return ERROR;

   CS_Force(0);

   muza_stat= SPY_Byte(0x0D); //����� ������������

   CS_Force(1);
   Timer1_Stop();
   return SUCCESS;
}

Status MUZA_two(void) {

  Timer1_Start(10);    // ����� ����� ���������� 1 ���.
  
  if (!WIN()) return ERROR;
 
  CS_Force(0);
  muza_stat= SPY_Byte(0x0B); //����� ������������
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;
}
