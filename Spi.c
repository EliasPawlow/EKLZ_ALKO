/* ========== Функции и структуры для работы с сопроцессором =============
 Здесь же работа с таймером, определяющим таймаут обмена с СП
для выявления сбойных ситуаций и зависаний
*/
#include "Defines.h"

unsigned char data_rd_SPI[MSG_SIZE_RD_SPI]; // Приемный буфер SPI
unsigned char data_wr_SPI[MSG_SIZE_WR_SPI]; // Буфер вывода SPI

CryptoCfg Crypto;                           // Блок управления сопроцессором

/*======== Внутренние функции ================================================*/

/*======= Ожидание готовности СП с заданным таймаутом =======*/
// Таймаут задается в миллисекундах
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

/*================== Публичные функции =====================================*/
//==== Инициализация SPI для обмена с сопроцессором =======================
/* Интерфейс используется для обмена с сопроцессором не совсем стандартным
   образом, с использованием дополнительного сигнала WAIT. Поэтому сигнал CS
   будем формировать "вручную", с использованием порта общего назначения.
   Распределение сигналов по ножкам процессора в проекте без GSM:
                  CS0  -> P0.16 - выход
                  MOSI -> P0.18 - выход
                  MISO -> P0.17 - вход
                  SCK  -> P0.15 - выход
                  WAIT -> P0.22 - вход
*/
void Init_SPI(void) 
{
  PINSEL_CFG_Type PinCfg;
  SPI_CFG_Type    SPICfg;
 
    	PinCfg.Funcnum = 3;     // Альтернативная функция 3 (т.е. SPI)
	PinCfg.OpenDrain = 0;   // Не "Открытый коллектор"
	PinCfg.Pinmode = 0;     // Pull-up резистор
	PinCfg.Pinnum = 15;      // Пин 15
	PinCfg.Portnum = 0;     // Порт 0 
	PINSEL_ConfigPin(&PinCfg);   // Настройка P0.15 (SCK)
	PinCfg.Pinnum = 17;           // Пин 17
	PINSEL_ConfigPin(&PinCfg);   // Настройка P0.17 (MISO)
       	PinCfg.Pinnum = 18;           // Пин 18
	PINSEL_ConfigPin(&PinCfg);   // Настройка P0.18 (MOSI)
        PinCfg.Funcnum = 0;      // Обычный вход-выход (GPIO)
        PinCfg.Pinnum = CS_PIN;           // Пин 16
        PINSEL_ConfigPin(&PinCfg);   // Настройка P0.16 (CS0)
        GPIO_SetDir(0, (1<<CS_PIN), 1);  // Сделаем его выходом
        GPIO_SetValue(0, (1<<16));   // И сразу установим в "1"
        PinCfg.Portnum = WAIT_PORT;
        PinCfg.Pinnum = WAIT_PIN;
        PINSEL_ConfigPin(&PinCfg);   // Настройка P2.13 (WAIT)
        GPIO_SetDir(WAIT_PORT, (1<<WAIT_PIN), 0);  // Сделаем его входом
        /* TODO: В итоге кусок инициализации надо будет оптимизировать, 
        установить регистры PINSEL, PINMODE и др. за раз Ну и до
        кучи можно будет LPC_SPI->SPCR=0x0834; */
        
        SPICfg.Databit = SPI_DATABIT_8; // 8 бит длина единицы обмена
        SPICfg.CPHA  =  SPI_CPHA_FIRST;
        SPICfg.CPOL  =  SPI_CPOL_LO;           // 
        SPICfg.Mode = SPI_MASTER_MODE;         // режим MASTER
        SPICfg.DataOrder = SPI_DATA_MSB_FIRST; // Старший бит первый
        SPICfg.ClockRate = 4000000;            // Скорость шины 4 MHz
        SPI_Init(SPI0, &SPICfg);

        
}
//==================== Управление сигналом CS0 =============================
void CS_Force(uint32_t state)
{
uint32_t tmp;

	if (state){
        // Перед убиранием CS дадим крипто принять последний байт посылки
        for (tmp = 50; tmp; tmp--);
	GPIO_SetValue(0, (1<<16));                                    
	}
        else
        {
		GPIO_ClearValue(0, (1<<16));
                for (tmp = 50; tmp; tmp--);
       // После сыставления CS в 0 дадим крипто это увидеть и перейти к приему 
       // первого байта
	}
}

//==================== Управление сигналом CS0 с анализом сигнала WAIT======
void CS_Force_WAIT(uint32_t state)
{
//TODO Анализ сигнала WAIT
uint32_t dwPortStatus;
uint32_t tmp;

        dwPortStatus = GPIO_ReadValue(0); // считать данные из 0 порта, получить значение выходов порта 0
	if (state){
        // Перед убиранием CS дадим крипто принять последний байт посылки
                for (tmp = 50; tmp; tmp--);
                while(dwPortStatus & (1<<WAIT_PIN))  {/*NOP*/}                     //Если WAIT_SIG=HI(неактивный уровень), то не нужно устанавливать сигнал SS(CS0), нужно ждать
		  GPIO_SetValue(0, (1<<16));                                    
	}
        else
        {
		GPIO_ClearValue(0, (1<<16));
                for (tmp = 50; tmp; tmp--);
       // После сыставления CS в 0 дадим крипто это увидеть и перейти к приему 
       // первого байта
	}
}
//=================== Команда криптопроцессору ============================

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
// ============= Передача одного байта в SPI ==============================
uint8_t SPY_Byte (uint8_t byte)
{
  volatile uint32_t tmp;
  SPI0->SPDR = byte;
  while (!((tmp = SPI0->SPSR) & SPI_SPSR_SPIF)); // Ждем завершения передачи байта
  for (tmp = 50; tmp; tmp--);
  
  byte = SPI0->SPDR;                             // Загрузим ответный байт
  return byte;
}
