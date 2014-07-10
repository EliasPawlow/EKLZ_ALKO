#include "Defines.h"
#include "modem.h"

unsigned char data_rd_UART[MSG_SIZE_RD]; // Входной буфер от кассы
unsigned char data_wr_UART[MSG_SIZE_WR]; // Выходной буфер "в кассу"
uint8_t len_msg_in;                //длина сообщения 
uint8_t len_msg_out;
BUF_Status buf_status;    // Глобальная переменная, по ней определяем состояние 
                          // приема и необходимость обработки команды
unsigned char time_out;   // Переменная, хранящая текущий таймаут
UART_CFG_Type UART0_Cfg;  // Структура, хранящая параметры UART0
UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // Структура упр. FIFO для UART1

//NEW
uint8_t UART_RCV_COUNT;   //Количество принятых байт
uint8_t UART_DONE;        //состояние UART - > Сообщение принято

//====== Обработчик прерываний UART0 ========================================
void UART0_IRQHandler(void) // прерывание на ввод данных из кассы
{
  unsigned char rd_data;
  static unsigned char *buf_point;
  Int_UART0_Disable(); 
  
  Channel = UART;  
  //Timer_Start();
          
          if(!UART_RCV_COUNT) 
            buf_point = &data_rd_UART[0];   //если не пришло еще данных, указатель в нуле

            if(!UART_DONE) rd_data = UART_ReceiveByte(UART0); 
            if(!UART_DONE) UART_RCV_COUNT++;
            if(!UART_DONE) *buf_point++ = rd_data;
	    if(!UART_DONE) Timer_Restart();
   Int_UART0_Enable(); 
            
}

//====== Подпрограмма загрузки начальных параметров портов ===================
void Init_Uart_Default(void) {
  time_out = 0;                     // переменная хранится в блоке Timer
  UART_ConfigStructInit(&UART0_Cfg);

  UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
}
//==== Инициализация UART0 - обмен с кассой ===================================
void Init_Uart0(void) {

  PINSEL_CFG_Type PinCfg;
  
  	PinCfg.Funcnum = 1;     // Альтернативная функция 1 (т.е. UART0)
	PinCfg.OpenDrain = 0;   // Не "Открытый коллектор"
	PinCfg.Pinmode = 0;     // Pull-up резистор
	PinCfg.Pinnum = 2;      // Пин 2
	PinCfg.Portnum = 0;     // Порт 0 
	PINSEL_ConfigPin(&PinCfg);   // Настройка P0.2 (TXD0)
	PinCfg.Pinnum = 3;           // Пин 3
	PINSEL_ConfigPin(&PinCfg);   // Настройка P0.3 (RXD0)

  UART_Init(UART0,&UART0_Cfg);   // инициализация порта
  UART_TxCmd(UART0, ENABLE); // Разрешение передачи
  UART_IntConfig(UART0, UART_INTCFG_RBR, ENABLE);
  // Разрешение прерывания по приходу символа

}

//===== Разрешение прерываний от UART0 =======================================
void Int_UART0_Enable(void) {
  
              NVIC_EnableIRQ(UART0_IRQn); // Разрешаем прерывание от UART0
}
//===== Запрет прерываний от UART0 ===========================================
void Int_UART0_Disable(void) {
  
              NVIC_DisableIRQ(UART0_IRQn); // Запрещаем прерывание от UART0

}


//===== Подпрограмма вычисления контрольной суммы ============================
uint8_t Get_CRC(unsigned char* point, uint32_t len) {
  uint8_t crc = 0;
  while (len) {
    crc ^= *point++;
    len--;
  }
  return crc;
}
                                                                           
void UART_refresh()
{
  UART_RCV_COUNT = 0;
  UART_DONE=0;
  Timer_Start();
}
