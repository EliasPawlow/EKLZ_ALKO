#include "lpc17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
//==================== Общие определения
#define   SPI0    LPC_SPI
#define   UART0   LPC_UART0
#define   UART1   LPC_UART1
#define   TIMER0  LPC_TIM0
#define   TIMER1  LPC_TIM1
//============= Только для GPIO, которые в LPC17xx лежат в SRAM================
#define PERIPH_BB_BASE 0x22000000
#define PERIPH_BASE    0x20000000

#define R_PORT0(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO0_BASE+0x14-PERIPH_BASE) * 0x20) + (i*4))))
#define R_PORT1(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO1_BASE+0x14-PERIPH_BASE) * 0x20) + (i*4))))
#define R_PORT2(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO2_BASE+0x14-PERIPH_BASE) * 0x20) + (i*4))))

#define C_PORT0(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO0_BASE+0x1C-PERIPH_BASE) * 0x20) + (i*4))))
#define C_PORT1(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO1_BASE+0x1C-PERIPH_BASE) * 0x20) + (i*4))))
#define C_PORT2(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO2_BASE+0x1C-PERIPH_BASE) * 0x20) + (i*4))))

#define S_PORT0(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO0_BASE+0x18-PERIPH_BASE) * 0x20) + (i*4))))
#define S_PORT1(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO1_BASE+0x18-PERIPH_BASE) * 0x20) + (i*4))))
#define S_PORT2(i) (*((__O unsigned long *) (PERIPH_BB_BASE + ((LPC_GPIO2_BASE+0x18-PERIPH_BASE) * 0x20) + (i*4))))
//=============================================================================

#define PWRKEY_ON          S_PORT2(8)=1
#define PWRKEY_OFF         C_PORT2(8)=1
#define CS_ON              C_PORT0(16)=1  // Установка 0 !
#define CS_OFF             S_PORT0(16)=1  // Установка единицы
//#define WAIT               R_PORT0(22) // 1756
#define WAIT               R_PORT2(13)    // 1768
#define STATUS             R_PORT2(4)  // 

#define STATUS_PORT   2 // Порт и бит, связанный с линией статуса модема
#define STATUS_PIN   4
#define PWR_PORT   2    // Порт и бит кнопки POWER модема
#define PWR_PIN   8
#define CS_PIN    16
#define CS_PORT   0

typedef enum {
	UART		= 0,    // Работает UART
        I2C                     // Работает I2C
} CAN_Status;         // Тип интерфейса обмена с кассой

extern  CAN_Status Channel; // Тип используемого для обмена с ККМ интерфейса

typedef enum {
	BUF_EMPTY		= 0, 	/* Буфер пуст, приема нет */
	BUF_PROGRESS,		        /*!< Обнаружен STX, начат прием */
	BUF_DONE,		     	/*!< Правильно принято сообщение */
        BUF_LAST_MSG                    /* Получен запрос на повторную передачу */
} BUF_Status;           // Возможные состояния буфера приема

//==================== Переменные, связанные с UART0 - касса ==================
#define MSG_SIZE_RD 	        256     //Размер приемного буфера из кассы
#define MSG_SIZE_WR 	        256     //Размер буфера передачи в кассу

#define STX   0x02
#define ACK   0x06
#define NACK  0x15
#define ENQ   0x05

extern unsigned char data_rd_UART[MSG_SIZE_RD]; // Приемный буфер - из кассы
extern unsigned char data_wr_UART[MSG_SIZE_WR]; // Выходной буфер - в кассу
extern uint8_t len_msg_in;                //длина сообщения 
extern uint8_t len_msg_out;


extern BUF_Status buf_status;        // Состояние буфера приема от кассы
extern unsigned char time_out;   // Переменная, хранящая текущий таймаут

extern uint8_t UART_DONE;
extern uint8_t UART_TMIO;
extern uint8_t UART_RCV_COUNT; 

extern uint8_t CMD_RCV_COUNT;
extern unsigned char cmd_buffer[256];//буфер ответных кодов команд.

extern UART_CFG_Type UART0_Cfg; //Блок конфигурации UART0
extern UART_CFG_Type UART1_Cfg; //Блок конфигурации UART1
extern UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // Структура упр. FIFO для UART1
//==================== Переменные, связанные с UART1 - GSM  ==================


//==================== Переменные, связанные со SPI - криптопроцессор =========
//#define WAIT_PIN          22                // Маска для сигнала WAIT  1768
//#define WAIT_PORT         0                 // Порт сигнала WAIT
#define WAIT_PIN          13                // Маска для сигнала WAIT 1754
#define WAIT_PORT         2                 // Порт сигнала WAIT



#define MSG_SIZE_RD_SPI 	256    //Размер буфера приема из криптопроц.
#define MSG_SIZE_WR_SPI 	256     //Размер буфера записи в криптопроц.
#define MAX_COMMAND             (M_FPO+1) // Максимально возможный номер команды
                                        // криптопроцессору
// Команды криптопроцессору
#define M_DESTROY     0       // Полное стирание памяти
#define M_DUMMY       1       // Пока нет данных
#define M_WRITE       2       // Команда записи в память
#define M_READ        3       // Команда сквозного чтения памяти
#define M_ERASE       4       // Команда стирания сектора
#define M_STATUS      5       // Чтение состояния КС
#define M_NOP         6       // Нет операции (аналогично 5)
#define M_WRBUF       7       // Запись в буфер
#define M_RDBUF       8       // Чтение буфера
#define M_SET0        9       // Команда установки 0-го банка памяти
#define M_SET1        10      // Команда установки 1-го банка памяти
#define M_FPO         11      // Запись в буфер фрагмента ФПО


extern unsigned char data_rd_SPI[MSG_SIZE_RD_SPI];
extern unsigned char data_wr_SPI[MSG_SIZE_WR_SPI];

typedef struct {
          uint8_t   c_status;     // Статус, полученный в ответ на команду
          uint32_t  c_address;    // Адрес во флеш-памяти
          uint32_t  c_len;        // Количество передаваемых данных
          uint8_t   *c_pointer;   // Указатель на буфер приема-передачи
} CryptoCfg;

extern CryptoCfg Crypto;          // Блок управления криптопроцессором

// Блок управления АЛКО
#define INSTALL       0x80
#define ACTIVATE      0x40
#define DATE_OUT      0x20
#define MEM_FULL      0x10
#define MEM_NEAR_FULL 0x08
#define FPO           0x04
#define CRYPT         0x02
#define ERR           0x01

//==================== Переменные, связанные с UART1 - GSM ====================
#define MODEM_INT_ENABLE  Int_UART1_Enable()  //Разрешение прерывания порта модема

//================== Определения, связанные с модемом ========================

/*============ Прототипы функций =================================*/
/*================== Init.c ======================================*/
void Init_GPIO(void);
/* =========   Модуль Uart.c ====================================*/
void Init_Uart0(void);        // Начальная инициализация порта UART0
void Init_Uart1(void);        // Начальная инициализация порта UART1
void Init_Uart_Default(void); // Загрузка параметров 9600 8 1 NONE
void Int_UART0_Enable(void);  // Разрешение прерывания от UART0
void Int_UART0_Disable(void); // Запрет прерывания от UART0
void Int_UART1_Enable(void);  // Разрешение прерывания от UART1
void Int_UART1_Disable(void); // Запрет прерывания от UART1
uint8_t Get_CRC(unsigned char*, uint32_t); // Контрольная сумма кассовых сообщ.
void UART_refresh();           // Сброс флагов UART и разрешение приема сообщений
/* =========   Модуль Timer.c ===================================*/
void Timer_Init(void);        // Начальная инициализация, интервал 1 ms
void Timer_Restart(void);     // Сброс текущего интервала
void Timer_Start(void);       // Запуск таймера
void Timer_Stop(void);        // Остановка таймера
void Init_Timer0 (void);      // Инициализация таймера 0
void Timer0_Start(uint32_t Delay); // Запуск таймера 0
void Timer0_Stop(void);       // Остановка таймера 0
Status Timer0_Status(void);   // Чтение состояния таймера 0
void Init_Timer1 (void);      // Инициализация таймера 1
void Timer1_Start(uint32_t Delay); // Запуск таймера 1
void Timer1_Stop(void);       // Остановка таймера 1
Status Timer1_Status(void);   // Чтение состояния таймера 1
void Wait(uint32_t Delay);    // Отработка задержки в миллисекундах
/* =========   Модуль Spi.c ====================================*/
void Init_SPI(void);          // Начальная инициализация SPI интерфейса
void CS_Force(uint32_t); // Управление сигналом CS0
void Init_Timer0 (void);      // Инициализация таймера обработки таймаута
uint8_t SPY_Byte (uint8_t byte); //Передача одного байта в SPY
Status Crypto_Command(uint8_t c_command); //Запрос выполнения команды криптопроц.
Status W_Wait(uint32_t Delay);

/* =========   Модуль Command.c ====================================*/
void FPO_check (void);          //проверка, надо ли отдать FPO
Status SendRcvdCmd();           //передает на СП принятую по UART последовательность байт
/* =========   Модуль I2C.c ====================================*/
void I2CInit_0(void);         // Инициализация интерфейса I2C
/* ==========  Модуль Command.c ================================*/
// Обработка запросов от ККМ
typedef enum {
  RET_OK = 0,     // Успешное выполнение команды
  ERR_FORMAT,     // некорректный формат или параметр команды
  ERR_STAT,       // некорректное состояние ЭКЛЗ
  ERR_HARD,       // авария ЭКЛЗ
  ERR_KRYPTO,     // авария КС (Криптографического сопроцессора)
  ERR_LIVE,       // исчерпан временной ресурс использования ЭКЛЗ
  ERR_FULL,       // ЭКЛЗ переполнена
  ERR_DATATIME,   // неверные дата или время
  ERR_NODATA,     // нет запрошенных данных
  ERR_OVER,       // переполнение (отрицательный итог документа, слишком много отделов 
                  // для клиента);
  ERR_NO_ANSWER,       // нет ответа от СП;
  ERR_WARN = 0x80 // ЭКЛЗ близка к заполнению (предупреждение)
} RET_Code;             // Коды возврата (реакция на команду ККМ)

void command(void);
void command_old(void);
void Init_CRYPTO(void);

