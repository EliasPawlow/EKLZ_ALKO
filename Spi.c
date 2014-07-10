/* ========== ������� � ��������� ��� ������ � ������������� =============
 ����� �� ������ � ��������, ������������ ������� ������ � ��
��� ��������� ������� �������� � ���������
*/
#include "Defines.h"

unsigned char data_rd_SPI[MSG_SIZE_RD_SPI]; // �������� ����� SPI
unsigned char data_wr_SPI[MSG_SIZE_WR_SPI]; // ����� ������ SPI

CryptoCfg Crypto;                           // ���� ���������� �������������

/*======== ���������� ������� ================================================*/

/*======= �������� ���������� �� � �������� ��������� =======*/
// ������� �������� � �������������
Status W_Wait(uint32_t Delay) {

uint32_t Port;

  Timer0_Start(Delay);

  while (!Timer0_Status()) {
    Port = GPIO_ReadValue(WAIT_PORT);
    if (!(Port & (1<<WAIT_PIN))) {
      Timer0_Stop();
      return SUCCESS;
    } else {
      Port = 0;
    }
  } 
  Timer0_Stop();
  return ERROR;
}

/*================== ��������� ������� =====================================*/
//==== ������������� SPI ��� ������ � ������������� =======================
/* ��������� ������������ ��� ������ � ������������� �� ������ �����������
   �������, � �������������� ��������������� ������� WAIT. ������� ������ CS
   ����� ����������� "�������", � �������������� ����� ������ ����������.
   ������������� �������� �� ������ ���������� � ������� ��� GSM:
                  CS0  -> P0.16 - �����
                  MOSI -> P0.18 - �����
                  MISO -> P0.17 - ����
                  SCK  -> P0.15 - �����
                  WAIT -> P0.22 - ����
*/
void Init_SPI(void) 
{
  PINSEL_CFG_Type PinCfg;
  SPI_CFG_Type    SPICfg;
 
    	PinCfg.Funcnum = 3;     // �������������� ������� 3 (�.�. SPI)
	PinCfg.OpenDrain = 0;   // �� "�������� ���������"
	PinCfg.Pinmode = 0;     // Pull-up ��������
	PinCfg.Pinnum = 15;      // ��� 15
	PinCfg.Portnum = 0;     // ���� 0 
	PINSEL_ConfigPin(&PinCfg);   // ��������� P0.15 (SCK)
	PinCfg.Pinnum = 17;           // ��� 17
	PINSEL_ConfigPin(&PinCfg);   // ��������� P0.17 (MISO)
       	PinCfg.Pinnum = 18;           // ��� 18
	PINSEL_ConfigPin(&PinCfg);   // ��������� P0.18 (MOSI)
        PinCfg.Funcnum = 0;      // ������� ����-����� (GPIO)
        PinCfg.Pinnum = CS_PIN;           // ��� 16
        PINSEL_ConfigPin(&PinCfg);   // ��������� P0.16 (CS0)
        GPIO_SetDir(0, (1<<CS_PIN), 1);  // ������� ��� �������
        GPIO_SetValue(0, (1<<16));   // � ����� ��������� � "1"
        PinCfg.Portnum = WAIT_PORT;
        PinCfg.Pinnum = WAIT_PIN;
        PINSEL_ConfigPin(&PinCfg);   // ��������� P2.13 (WAIT)
        GPIO_SetDir(WAIT_PORT, (1<<WAIT_PIN), 0);  // ������� ��� ������
        /* TODO: � ����� ����� ������������� ���� ����� ��������������, 
        ���������� �������� PINSEL, PINMODE � ��. �� ��� �� � ��
        ���� ����� ����� LPC_SPI->SPCR=0x0834; */
        
        SPICfg.Databit = SPI_DATABIT_8; // 8 ��� ����� ������� ������
        SPICfg.CPHA  =  SPI_CPHA_FIRST;
        SPICfg.CPOL  =  SPI_CPOL_LO;           // 
        SPICfg.Mode = SPI_MASTER_MODE;         // ����� MASTER
        SPICfg.DataOrder = SPI_DATA_MSB_FIRST; // ������� ��� ������
        SPICfg.ClockRate = 4000000;            // �������� ���� 4 MHz
        SPI_Init(SPI0, &SPICfg);

        
}
//==================== ���������� �������� CS0 =============================
void CS_Force(uint32_t state)
{
uint32_t tmp;

	if (state){
        // ����� ��������� CS ����� ������ ������� ��������� ���� �������
        for (tmp = 50; tmp; tmp--);
	GPIO_SetValue(0, (1<<16));                                    
	}
        else
        {
		GPIO_ClearValue(0, (1<<16));
                for (tmp = 50; tmp; tmp--);
       // ����� ����������� CS � 0 ����� ������ ��� ������� � ������� � ������ 
       // ������� �����
	}
}

//==================== ���������� �������� CS0 � �������� ������� WAIT======
void CS_Force_WAIT(uint32_t state)
{
//TODO ������ ������� WAIT
uint32_t dwPortStatus;
uint32_t tmp;

        dwPortStatus = GPIO_ReadValue(0); // ������� ������ �� 0 �����, �������� �������� ������� ����� 0
	if (state){
        // ����� ��������� CS ����� ������ ������� ��������� ���� �������
                for (tmp = 50; tmp; tmp--);
                while(dwPortStatus & (1<<WAIT_PIN))  {/*NOP*/}                     //���� WAIT_SIG=HI(���������� �������), �� �� ����� ������������� ������ SS(CS0), ����� �����
		  GPIO_SetValue(0, (1<<16));                                    
	}
        else
        {
		GPIO_ClearValue(0, (1<<16));
                for (tmp = 50; tmp; tmp--);
       // ����� ����������� CS � 0 ����� ������ ��� ������� � ������� � ������ 
       // ������� �����
	}
}
//=================== ������� ���������������� ============================

Status Crypto_Command(uint8_t c_command) 
{
uint8_t tmp;
uint8_t mass[512];
uint32_t i;

  if(c_command > M_FPO) return ERROR;
  
  if (!(W_Wait(1000))) return ERROR;
  
  switch (c_command) {
    
   case M_DESTROY:

     break;
          
    case M_WRITE:
                  break;
          
   case M_READ:
                  break;
          
   case M_ERASE:
                  break;
          
   case M_STATUS:
   case M_DUMMY:
   case M_NOP:
     CS_Force(0);
     Crypto.c_status = SPY_Byte(M_STATUS);
     CS_Force(1);
                  break;
          
   case M_WRBUF:
     for (i=0;i<511;i++) mass[i]=0;
     CS_Force(0);
     Crypto.c_status = SPY_Byte(M_WRBUF);
     for(i=0;i<500;i++){
       if (!W_Wait(1000)) { 
         i=511;
       } else {
         SPY_Byte(i);
         for (tmp = 50; tmp; tmp--);
       }
     }
     CS_Force(1);
     CS_Force(0);
     Crypto.c_status = SPY_Byte(M_RDBUF);

     for(i=0;i<500;i++){
       if (!W_Wait(1000)) {
         i = 511;
       } else {
         mass[i]=SPY_Byte(0xFF);
         for (tmp = 50; tmp; tmp--);
       }
     }
     CS_Force(1);

                  break;
          
   case M_RDBUF:
                  break;
          
   case M_SET0:
                  break;
          
   case M_SET1:
                  break;
          
   case M_FPO:
                  break;
          
  default:
                  break;
  }
  
  return SUCCESS;
}
// ============= �������� ������ ����� � SPI ==============================
uint8_t SPY_Byte (uint8_t byte)
{
  volatile uint32_t tmp;
  SPI0->SPDR = byte;
  while (!((tmp = SPI0->SPSR) & SPI_SPSR_SPIF)); // ���� ���������� �������� �����
  for (tmp = 50; tmp; tmp--);
  
  byte = SPI0->SPDR;                             // �������� �������� ����
  return byte;
}
