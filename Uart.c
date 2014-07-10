#include "Defines.h"
#include "modem.h"

unsigned char data_rd_UART[MSG_SIZE_RD]; // ������� ����� �� �����
unsigned char data_wr_UART[MSG_SIZE_WR]; // �������� ����� "� �����"
uint8_t len_msg_in;                //����� ��������� 
uint8_t len_msg_out;
BUF_Status buf_status;    // ���������� ����������, �� ��� ���������� ��������� 
                          // ������ � ������������� ��������� �������
unsigned char time_out;   // ����������, �������� ������� �������
UART_CFG_Type UART0_Cfg;  // ���������, �������� ��������� UART0
UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // ��������� ���. FIFO ��� UART1

//NEW
uint8_t UART_RCV_COUNT;   //���������� �������� ����
uint8_t UART_DONE;        //��������� UART - > ��������� �������

//====== ���������� ���������� UART0 ========================================
void UART0_IRQHandler(void) // ���������� �� ���� ������ �� �����
{
  unsigned char rd_data;
  static unsigned char *buf_point;
  Int_UART0_Disable(); 
  
  Channel = UART;  
  //Timer_Start();
          
          if(!UART_RCV_COUNT) 
            buf_point = &data_rd_UART[0];   //���� �� ������ ��� ������, ��������� � ����

            if(!UART_DONE) rd_data = UART_ReceiveByte(UART0); 
            if(!UART_DONE) UART_RCV_COUNT++;
            if(!UART_DONE) *buf_point++ = rd_data;
	    if(!UART_DONE) Timer_Restart();
   Int_UART0_Enable(); 
            
}

//====== ������������ �������� ��������� ���������� ������ ===================
void Init_Uart_Default(void) {
  time_out = 0;                     // ���������� �������� � ����� Timer
  UART_ConfigStructInit(&UART0_Cfg);

  UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
}
//==== ������������� UART0 - ����� � ������ ===================================
void Init_Uart0(void) {

  PINSEL_CFG_Type PinCfg;
  
  	PinCfg.Funcnum = 1;     // �������������� ������� 1 (�.�. UART0)
	PinCfg.OpenDrain = 0;   // �� "�������� ���������"
	PinCfg.Pinmode = 0;     // Pull-up ��������
	PinCfg.Pinnum = 2;      // ��� 2
	PinCfg.Portnum = 0;     // ���� 0 
	PINSEL_ConfigPin(&PinCfg);   // ��������� P0.2 (TXD0)
	PinCfg.Pinnum = 3;           // ��� 3
	PINSEL_ConfigPin(&PinCfg);   // ��������� P0.3 (RXD0)

  UART_Init(UART0,&UART0_Cfg);   // ������������� �����
  UART_TxCmd(UART0, ENABLE); // ���������� ��������
  UART_IntConfig(UART0, UART_INTCFG_RBR, ENABLE);
  // ���������� ���������� �� ������� �������

}

//===== ���������� ���������� �� UART0 =======================================
void Int_UART0_Enable(void) {
  
              NVIC_EnableIRQ(UART0_IRQn); // ��������� ���������� �� UART0
}
//===== ������ ���������� �� UART0 ===========================================
void Int_UART0_Disable(void) {
  
              NVIC_DisableIRQ(UART0_IRQn); // ��������� ���������� �� UART0

}


//===== ������������ ���������� ����������� ����� ============================
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
