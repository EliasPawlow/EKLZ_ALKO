#include "lpc17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
//==================== ����� �����������
#define   SPI0    LPC_SPI
#define   UART0   LPC_UART0
#define   UART1   LPC_UART1
#define   TIMER0  LPC_TIM0
#define   TIMER1  LPC_TIM1
//============= ������ ��� GPIO, ������� � LPC17xx ����� � SRAM================
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
#define CS_ON              C_PORT0(16)=1  // ��������� 0 !
#define CS_OFF             S_PORT0(16)=1  // ��������� �������
//#define WAIT               R_PORT0(22) // 1756
#define WAIT               R_PORT2(13)    // 1768
#define STATUS             R_PORT2(4)  // 

#define STATUS_PORT   2 // ���� � ���, ��������� � ������ ������� ������
#define STATUS_PIN   4
#define PWR_PORT   2    // ���� � ��� ������ POWER ������
#define PWR_PIN   8
#define CS_PIN    16
#define CS_PORT   0

typedef enum {
	UART		= 0,    // �������� UART
        I2C                     // �������� I2C
} CAN_Status;         // ��� ���������� ������ � ������

extern  CAN_Status Channel; // ��� ������������� ��� ������ � ��� ����������

typedef enum {
	BUF_EMPTY		= 0, 	/* ����� ����, ������ ��� */
	BUF_PROGRESS,		        /*!< ��������� STX, ����� ����� */
	BUF_DONE,		     	/*!< ��������� ������� ��������� */
        BUF_LAST_MSG                    /* ������� ������ �� ��������� �������� */
} BUF_Status;           // ��������� ��������� ������ ������

//==================== ����������, ��������� � UART0 - ����� ==================
#define MSG_SIZE_RD 	        256     //������ ��������� ������ �� �����
#define MSG_SIZE_WR 	        256     //������ ������ �������� � �����

#define STX   0x02
#define ACK   0x06
#define NACK  0x15
#define ENQ   0x05

extern unsigned char data_rd_UART[MSG_SIZE_RD]; // �������� ����� - �� �����
extern unsigned char data_wr_UART[MSG_SIZE_WR]; // �������� ����� - � �����
extern uint8_t len_msg_in;                //����� ��������� 
extern uint8_t len_msg_out;


extern BUF_Status buf_status;        // ��������� ������ ������ �� �����
extern unsigned char time_out;   // ����������, �������� ������� �������

extern uint8_t UART_DONE;
extern uint8_t UART_TMIO;
extern uint8_t UART_RCV_COUNT; 

extern uint8_t CMD_RCV_COUNT;
extern unsigned char cmd_buffer[256];//����� �������� ����� ������.

extern UART_CFG_Type UART0_Cfg; //���� ������������ UART0
extern UART_CFG_Type UART1_Cfg; //���� ������������ UART1
extern UART_FIFO_CFG_Type UARTFIFOConfigStruct;  // ��������� ���. FIFO ��� UART1
//==================== ����������, ��������� � UART1 - GSM  ==================


//==================== ����������, ��������� �� SPI - ��������������� =========
//#define WAIT_PIN          22                // ����� ��� ������� WAIT  1768
//#define WAIT_PORT         0                 // ���� ������� WAIT
#define WAIT_PIN          13                // ����� ��� ������� WAIT 1754
#define WAIT_PORT         2                 // ���� ������� WAIT



#define MSG_SIZE_RD_SPI 	256    //������ ������ ������ �� ����������.
#define MSG_SIZE_WR_SPI 	256     //������ ������ ������ � ����������.
#define MAX_COMMAND             (M_FPO+1) // ����������� ��������� ����� �������
                                        // ����������������
// ������� ����������������
#define M_DESTROY     0       // ������ �������� ������
#define M_DUMMY       1       // ���� ��� ������
#define M_WRITE       2       // ������� ������ � ������
#define M_READ        3       // ������� ��������� ������ ������
#define M_ERASE       4       // ������� �������� �������
#define M_STATUS      5       // ������ ��������� ��
#define M_NOP         6       // ��� �������� (���������� 5)
#define M_WRBUF       7       // ������ � �����
#define M_RDBUF       8       // ������ ������
#define M_SET0        9       // ������� ��������� 0-�� ����� ������
#define M_SET1        10      // ������� ��������� 1-�� ����� ������
#define M_FPO         11      // ������ � ����� ��������� ���


extern unsigned char data_rd_SPI[MSG_SIZE_RD_SPI];
extern unsigned char data_wr_SPI[MSG_SIZE_WR_SPI];

typedef struct {
          uint8_t   c_status;     // ������, ���������� � ����� �� �������
          uint32_t  c_address;    // ����� �� ����-������
          uint32_t  c_len;        // ���������� ������������ ������
          uint8_t   *c_pointer;   // ��������� �� ����� ������-��������
} CryptoCfg;

extern CryptoCfg Crypto;          // ���� ���������� �����������������

// ���� ���������� ����
#define INSTALL       0x80
#define ACTIVATE      0x40
#define DATE_OUT      0x20
#define MEM_FULL      0x10
#define MEM_NEAR_FULL 0x08
#define FPO           0x04
#define CRYPT         0x02
#define ERR           0x01

//==================== ����������, ��������� � UART1 - GSM ====================
#define MODEM_INT_ENABLE  Int_UART1_Enable()  //���������� ���������� ����� ������

//================== �����������, ��������� � ������� ========================

/*============ ��������� ������� =================================*/
/*================== Init.c ======================================*/
void Init_GPIO(void);
/* =========   ������ Uart.c ====================================*/
void Init_Uart0(void);        // ��������� ������������� ����� UART0
void Init_Uart1(void);        // ��������� ������������� ����� UART1
void Init_Uart_Default(void); // �������� ���������� 9600 8 1 NONE
void Int_UART0_Enable(void);  // ���������� ���������� �� UART0
void Int_UART0_Disable(void); // ������ ���������� �� UART0
void Int_UART1_Enable(void);  // ���������� ���������� �� UART1
void Int_UART1_Disable(void); // ������ ���������� �� UART1
uint8_t Get_CRC(unsigned char*, uint32_t); // ����������� ����� �������� �����.
void UART_refresh();           // ����� ������ UART � ���������� ������ ���������
/* =========   ������ Timer.c ===================================*/
void Timer_Init(void);        // ��������� �������������, �������� 1 ms
void Timer_Restart(void);     // ����� �������� ���������
void Timer_Start(void);       // ������ �������
void Timer_Stop(void);        // ��������� �������
void Init_Timer0 (void);      // ������������� ������� 0
void Timer0_Start(uint32_t Delay); // ������ ������� 0
void Timer0_Stop(void);       // ��������� ������� 0
Status Timer0_Status(void);   // ������ ��������� ������� 0
void Init_Timer1 (void);      // ������������� ������� 1
void Timer1_Start(uint32_t Delay); // ������ ������� 1
void Timer1_Stop(void);       // ��������� ������� 1
Status Timer1_Status(void);   // ������ ��������� ������� 1
void Wait(uint32_t Delay);    // ��������� �������� � �������������
/* =========   ������ Spi.c ====================================*/
void Init_SPI(void);          // ��������� ������������� SPI ����������
void CS_Force(uint32_t); // ���������� �������� CS0
void Init_Timer0 (void);      // ������������� ������� ��������� ��������
uint8_t SPY_Byte (uint8_t byte); //�������� ������ ����� � SPY
Status Crypto_Command(uint8_t c_command); //������ ���������� ������� ����������.
Status W_Wait(uint32_t Delay);

/* =========   ������ Command.c ====================================*/
void FPO_check (void);          //��������, ���� �� ������ FPO
Status SendRcvdCmd();           //�������� �� �� �������� �� UART ������������������ ����
/* =========   ������ I2C.c ====================================*/
void I2CInit_0(void);         // ������������� ���������� I2C
/* ==========  ������ Command.c ================================*/
// ��������� �������� �� ���
typedef enum {
  RET_OK = 0,     // �������� ���������� �������
  ERR_FORMAT,     // ������������ ������ ��� �������� �������
  ERR_STAT,       // ������������ ��������� ����
  ERR_HARD,       // ������ ����
  ERR_KRYPTO,     // ������ �� (������������������ ������������)
  ERR_LIVE,       // �������� ��������� ������ ������������� ����
  ERR_FULL,       // ���� �����������
  ERR_DATATIME,   // �������� ���� ��� �����
  ERR_NODATA,     // ��� ����������� ������
  ERR_OVER,       // ������������ (������������� ���� ���������, ������� ����� ������� 
                  // ��� �������);
  ERR_NO_ANSWER,       // ��� ������ �� ��;
  ERR_WARN = 0x80 // ���� ������ � ���������� (��������������)
} RET_Code;             // ���� �������� (������� �� ������� ���)

void command(void);
void command_old(void);
void Init_CRYPTO(void);

