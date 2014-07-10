#include "Defines.h"
#include "crypto.h"

uint8_t muza_stat;
char    muza_buff[256];  // Буфер для обмена с криптопроцессором
MAC_num MAC;

uint8_t muza_len;        // длина данных в буфере криптопроцессора

Status WIN (void);
// Локальная подпрограмма ожидания WAIT при наличии таймера
// Если не дождались, таймер останавливаем и CS убираем
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

//--------------------Активизация СП	MUZA_Act	0x01    D[0] = n = 17h, D[1…n] 

Status MUZA_Act(uint8_t len, char *ptr) {

  uint8_t tmp;

  Timer1_Start(1000);           // Установим таймаут 10 сек.

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
  
//---------------------Закрытие архива 	MUZA_Close	0x02    D[0] = n = 17h, D[1…n] 

Status MUZA_Close(uint8_t len, char *ptr) {

  

  Timer1_Start(1000);           // Установим таймаут 10 сек.

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

//---------------------Тест «Входной контроль»	 MUZA_IO_Control	0x03    Нет
Status MUZA_IO_Control(uint32_t delay) {

  Timer1_Start(delay);    // Будем ждать готовности delay/10 сек.

  if (!WIN()) return ERROR;

  CS_Force(0);
  
  muza_stat = SPY_Byte(0x03);   //
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;   
}
//---------------------MUZA_Test- пока неясно
// Команда начальной инициализации при тестировании
Status MUZA_Test(uint32_t delay) {
   Timer1_Start(delay);    // Будем ждать готовности delay/10 сек.
   
   if (!WIN()) return ERROR;
   
   CS_Force(0);
   muza_stat= SPY_Byte(0x03); //Дадим команду инициализации         ЭТО ОТЛАДКА???
   CS_Force(1);
   
   if (!WIN()) return ERROR;

   CS_Force(0);
   muza_stat = SPY_Byte(0x05); // Чтение статуса после инициализации
   CS_Force(1);
   
   Timer1_Stop();
//    if (muza_stat == 0xC0) 
      return SUCCESS;
    return ERROR;
}

//---------------------Запрос версии ПО СП	MUZA_Ver_Software	0x04    Нет
Status MUZA_Ver_Software(uint8_t len, char *out) {
  Timer1_Start(1000);           // Установим таймаут 10 сек.
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


//---------------------Запрос байта состояния	MUZA_Status	        0x05    Нет
// Запрос состояния криптопроцессора. Если ERROR, то авария, иначе muza_stat 

Status MUZA_Status_new()
{
  //подготовка данных
  UART_RCV_COUNT = 4;
  data_rd_UART[0] = 0x2;
  data_rd_UART[1] = 0x1;
  data_rd_UART[2] = 0x5;
  data_rd_UART[3] = 0x4;
  //отправка данных
  if(SendRcvdCmd())
  {
    muza_stat = cmd_buffer[0];
    return SUCCESS;
  }
  return ERROR;
}

Status MUZA_Status(uint32_t delay) {
  //заполнить read buffer и обратиться к написанной процедуре
 uint32_t  len = 50;
 uint32_t  count=0;
  Timer1_Start(delay);    // Будем ждать готовности delay/10 сек.
  if (!WIN()) return ERROR;

  CS_Force(0);
  muza_stat = SPY_Byte(0x02);
  SPY_Byte(0x01);
  SPY_Byte(0x05);   //код запроса
  SPY_Byte(0x04);
  CS_Force(1);
  Timer1_Stop();

  while(!WIN());
   Timer1_Start(delay);    // Будем ждать готовности delay/10 сек.
  if (!WIN()) return ERROR;
  CS_Force(0);
 
 count = 0;   
  while (len) {
      if (!WIN()) return ERROR;
      data_wr_UART[count] = SPY_Byte(count);
      //TODO проверить что возвращает. BP
      count++;

      len--;
    }
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;   
}


//---------------------Запрос регистрационного номера 	MUZA_num	0x07    Nom_Excise[0…4]

Status MUZA_num(uint32_t delay) { // Чтение серийного номера
  uint32_t  len = 254;
  
   Timer1_Start(delay);         // Будем ждать готовности delay/10 сек.
  
   if (!WIN()) return ERROR;
   CS_Force(0);

   muza_stat = SPY_Byte(0x02); //Дадим команду чтения номера { 0x02 0x01 0x07 0x06 }
//    if (!WIN()) return ERROR;
  SPY_Byte(0x01);
//    if (!WIN()) return ERROR;
  SPY_Byte(0x07);              
//    if (!WIN()) return ERROR;
  SPY_Byte(0x06);
 //   if (!WIN()) return ERROR;
     CS_Force(1);       // снимем сигнал CS и дадим принять команду до конца 

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

//---------------------Регистрация документа	MUZA_Save	0x08    D[0] = n,D[1…n] (20 ? n ? 240)

Status MUZA_Save(uint8_t len, char *ptr) {

  
  Timer1_Start(1000);           // Установим таймаут 10 сек.

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

  
//---------------------Ввод  подтверждения	MUZA_check	0x09    Nom_MAC[0…2], Conf_MAC[0,1]

Status MUZA_check(char *in, char *out) {

  Timer1_Start(1000);           // Установим таймаут 10 сек.
  if (!WIN()) return ERROR;
  CS_Force(0);
  muza_stat = SPY_Byte(0x09);
  SPY_Byte(*in++);      // Три байта адреса
  SPY_Byte(*in++);
  SPY_Byte(*in++);
  SPY_Byte(*in++);      //  и два байта кода  подтверждения
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


//---------------------Запрос документа по номеру MAC	MAC.muza_mac ???        0x0A	Nom_MAC[0…2]	
//---------------------Нулевое значение соответствует запросу документа
//---------------------с минимальным неподтвержденным номером MAC

Status MUZA_Doc(uint8_t *count, unsigned char *ptr) {
  
  uint8_t len;
 
  Timer1_Start(1000);           // Установим таймаут 10 сек.

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

//---------------------Тест целостности ПО	MUZA_Integrity_Software	0x0B	IV[0…7]

//---------------------Чтение буфера 	MUZA_READ_BUF	0x0C	Нет

//---------------------Блок ПО КП	Block_FPO	0x0D	Count[0,1], D[0…63]


// Команда передачи ФПО в криптопроцессор
Status MUZA_FPO(uint32_t delay) {
    uint32_t i,count;
    uint8_t byte_send, LRC_msg;
    uint32_t count2 = 0x2000;            //правильно 0x2000
 
        Timer1_Start(delay);         // Будем ждать готовности delay/10 сек.
       for (count=0; count < count2; count++) {        
 //    while ( count < count2) {
        if (!WIN()) 
              return ERROR;    // Нет готовности, ждать нечего
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // это STX
        SPY_Byte(0x43);                //байт длины
		
	SPY_Byte(0x0D);           //Дадим команду передачи ФПО  - код команды и далее блок DATA-64 байта	
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
              return ERROR;    // Нет готовности, ждать нечего
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // это STX
        SPY_Byte(0x43);                //байт длины
		
	SPY_Byte(0x0D);           //Дадим команду передачи ФПО  - код команды и далее блок DATA-64 байта	
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
              return ERROR;    // Нет готовности, ждать нечего
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // это STX
        SPY_Byte(0x03);                //байт длины
		
	SPY_Byte(0x0D);           //Дадим команду передачи ФПО  - код команды и далее блок DATA-64 байта	
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
              return ERROR;    // Нет готовности, ждать нечего
//        printf("Len = %i\n",len);
        CS_Force(0);
        muza_stat = SPY_Byte(0x02); // это STX
        SPY_Byte(0x03);                //байт длины
		
	SPY_Byte(0x0D);           //Дадим команду передачи ФПО  - код команды и далее блок DATA-64 байта	
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


//---------------------Чтение архива	MUZA_Dump	0x0E	А[3..0]

Status MUZA_Dump(uint32_t len, char *addr, char *ptr) {
  Timer1_Start(1000);           // Установим таймаут 10 сек.

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

//---------------------Стирание памяти	MUZA_erase	0x0F	Нет

// Стирание памяти. Если время стирания или выход в готовность превышает 400, то ошибка
Status MUZA_erase(void) {
  Timer1_Start(4000); // 400 секунд на стирание всей памяти

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
     Timer1_Start(10);    // Будем ждать готовности 1 сек.

     if (!WIN()) return ERROR;

   CS_Force(0);

   muza_stat= SPY_Byte(0x0D); //Дадим переключения

   CS_Force(1);
   Timer1_Stop();
   return SUCCESS;
}

Status MUZA_two(void) {

  Timer1_Start(10);    // Будем ждать готовности 1 сек.
  
  if (!WIN()) return ERROR;
 
  CS_Force(0);
  muza_stat= SPY_Byte(0x0B); //Дадим переключения
  CS_Force(1);

  Timer1_Stop();
  return SUCCESS;
}
