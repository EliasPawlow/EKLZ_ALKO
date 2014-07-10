
#define MIN_DELAY  3000   // Задержка в 3 секунды
#define RING_SIZE 	        2048     //Размер приемного буфера из модема



typedef enum {
  S2400 = 0,
  S4800,
  S9600,
  S14400
} RATES;


/*================== Входной буфер модема и связанные переменные ============*/
extern unsigned char modem_ring[RING_SIZE];        // Входной буфер модема
extern unsigned char *ring_head;       // Указатель на место для приема следующего
                                // символа
extern unsigned char *ring_tail;       // Указатель на следующий необработанный символ
extern uint8_t        CR_count;        // Счетчик принятых символов ВК (возврат
                                // каретки, т.е. принятых строк)

