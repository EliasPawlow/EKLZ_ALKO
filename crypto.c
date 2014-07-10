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

  uint8_t tmp;

  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;  CS_Force(0);

  muza_stat = SPY_Byte(0x04);  if (!WIN()) return ERROR;

  SPY_Byte(len);          if (!WIN()) return ERROR;

  while (len) {
    if (!WIN()) return ERROR;
    tmp = SPY_Byte(*ptr++);
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

Status MUZA_Status_new()
{
  //���������� ������
  UART_RCV_COUNT = 4;
  data_rd_UART[0] = 0x2;
  data_rd_UART[1] = 0x1;
  data_rd_UART[2] = 0x5;
  data_rd_UART[3] = 0x4;
  //�������� ������
  return SendRcvdCmd();
}

Status MUZA_Status(uint32_t delay) {
  //��������� read buffer � ���������� � ���������� ���������
 uint32_t  len = 50;
 uint32_t  count=0;
 unsigned char  bb[12];
  Timer1_Start(delay);    // ����� ����� ���������� delay/10 ���.
  if (!WIN()) return ERROR;

  CS_Force(0);
  muza_stat = SPY_Byte(0x02);
  SPY_Byte(0x01);
  SPY_Byte(0x05);   //��� �������
  SPY_Byte(0x04);
  CS_Force(1);
  Timer1_Stop();
    /*for (count=0;count<254;count++) {
    data_wr_UART[count] =0x33;
  }*/
  while(!WIN());
   Timer1_Start(delay);    // ����� ����� ���������� delay/10 ���.
  if (!WIN()) return ERROR;
  CS_Force(0);
 
  /*
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
 */
  
 count = 0;   
  while (len) {
      if (!WIN()) return ERROR;
      data_wr_UART[count] = SPY_Byte(count);
      //TODO ��������� ��� ����������. BP
      count++;

      len--;
    }
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;   
}



//--------------------������ ��������� ��       MUZA_Full_Status	0x06    ���


//---------------------������ ���������������� ������ 	MUZA_num	0x07    Nom_Excise[0�4]

Status MUZA_num(uint32_t delay) { // ������ ��������� ������
  uint32_t  len = 254;
  uint32_t  count = 0;
  
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
/*  
      data_wr_UART[count]=SPY_Byte(0x00);
      count++;
      data_wr_UART[count]= SPY_Byte(0x00);
      len = data_wr_UART[count]+1;
      count++;
*/
/*     
        data_wr_UART[150]=0xf0;
        data_wr_UART[151]=0xf0;
        data_wr_UART[152]=0xf0;
        data_wr_UART[153]=0xf0;
*/
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

//  char  *ptr = muza_buff;
  uint8_t tmp;
  
  Timer1_Start(1000);           // ��������� ������� 10 ���.

  if (!WIN()) return ERROR;

  CS_Force(0);    
  muza_stat = SPY_Byte(0x08); 
  if (!WIN()) return ERROR;   SPY_Byte(len); 
  if (!WIN()) return ERROR;

  while (len) {
    if (!WIN()) return ERROR;
    tmp = SPY_Byte(*ptr++);
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

//---------------------���� ����������� ��	MUZA_Integrity_Software	0x0B	IV[0�7]

//---------------------������ ������ 	MUZA_READ_BUF	0x0C	���

//---------------------���� �� ��	Block_FPO	0x0D	Count[0,1], D[0�63]


// ������� �������� ��� � ���������������
Status MUZA_FPO(uint32_t delay) {
    uint32_t i,count;
    uint8_t byte_send, LRC_msg;
    uint32_t count2 = 0x2000;            //��������� 0x2000
 
        Timer1_Start(delay);         // ����� ����� ���������� delay/10 ���.
       for (count=0; count < count2; count++) {        
 //    while ( count < count2) {
        if (!WIN()) 
              return ERROR;    // ��� ����������, ����� ������
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // ��� STX
        SPY_Byte(0x43);                //���� �����
		
	SPY_Byte(0x0D);           //����� ������� �������� ���  - ��� ������� � ����� ���� DATA-64 �����	
		LRC_msg = 0x4E;        // =0x43^0x0D
		byte_send = count & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                     
			byte_send = (count >> 8) & 0xFF;
			LRC_msg ^= byte_send;
        SPY_Byte(byte_send);
         for (i=0; i<64; i++) {        
  				byte_send = i & 0xFF;
	            LRC_msg ^= byte_send;
             SPY_Byte(byte_send);     
         }
         SPY_Byte(LRC_msg);
        CS_Force(1);
        
        if (!WIN()) 
              return ERROR;    // ��� ����������, ����� ������
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // ��� STX
        SPY_Byte(0x43);                //���� �����
		
	SPY_Byte(0x0D);           //����� ������� �������� ���  - ��� ������� � ����� ���� DATA-64 �����	
		LRC_msg = 0x4E;        // =0x43^0x0D
		byte_send = count & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                     
			byte_send = (count >> 8) & 0xFF;
			LRC_msg ^= byte_send;
        SPY_Byte(byte_send);
         for (i=0; i<64; i++) {        
  				byte_send = i & 0xFF;
	            LRC_msg ^= byte_send;
             SPY_Byte(byte_send);     
         }
         SPY_Byte(LRC_msg);
        CS_Force(1);
        
  //       count--;      
        }
        count = count2;
        if (!WIN()) 
              return ERROR;    // ��� ����������, ����� ������
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // ��� STX
        SPY_Byte(0x03);                //���� �����
		
	SPY_Byte(0x0D);           //����� ������� �������� ���  - ��� ������� � ����� ���� DATA-64 �����	
		LRC_msg = 0x0E;        // =0x03^0x0D
		byte_send = count & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                    
		byte_send = (count>> 8) & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                 
        SPY_Byte(LRC_msg);
        CS_Force(1);
        if (!WIN()) 
              return ERROR;    // ��� ����������, ����� ������
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // ��� STX
        SPY_Byte(0x03);                //���� �����
		
	SPY_Byte(0x0D);           //����� ������� �������� ���  - ��� ������� � ����� ���� DATA-64 �����	
		LRC_msg = 0x0E;        // =0x03^0x0D
		byte_send = count & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                    
		byte_send = (count>> 8) & 0xFF;
		LRC_msg ^= byte_send;
        SPY_Byte(byte_send);                 
        SPY_Byte(LRC_msg);
        CS_Force(1);
   
          
     Timer1_Stop();
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



//�������� ��������� � ��
Status MUZA_TransmitMSG( uint8_t len_msg_in, unsigned char *ptr) {

uint8_t count = 0;
unsigned char byte;
  
  Timer1_Start(1000);           // ��������� ������� 10 ���.
  CS_Force(0);

  while (len_msg_in--) 
  {
      byte = SPY_Byte(*ptr++);
      if (!WIN()) return ERROR;
      count++;
  }
  
    CS_Force(1);
    Timer1_Stop();
  return SUCCESS;
} 
  
//����� ��������� �� ��
Status MUZA_ReceiveMSG(uint8_t len_msg_out, unsigned char *ptr) {
uint8_t  count = 0;
unsigned char   byte;

  Timer1_Start(1000);
  CS_Force(0);

  count = 0;
  while (len_msg_out--) {
    if (!WIN()) return ERROR;
      *ptr++ = SPY_Byte(0x0F);
      count++;   
  }
  len_msg_out = count;
    Timer1_Stop();
  CS_Force(1);
  return SUCCESS;
}


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

// �������� ��������� � ���������������.
// @param szMessage - ������ ������������� ��������� � ������
// @param p_InMessage - ��������� �� ������ � ���������� �������
// @param p_OutMessage - ��������� �� ������ � �����������
Status MUZA_SendMessage(uint8_t szMessage,uint8_t *p_InMessage, uint8_t *p_OutMessage)
{
  CS_Force_WAIT(0);
    while(szMessage--)
    {
      *p_OutMessage++ = *p_InMessage++;
      if(!WIN()) return ERROR;
    }
  
  CS_Force_WAIT(1);
  return SUCCESS;
}