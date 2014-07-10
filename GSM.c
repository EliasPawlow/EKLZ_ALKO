#include "gsm.h"

unsigned char modem_ring[RING_SIZE];        // Входной буфер модема
unsigned char *ring_head;       // Указатель на место для приема следующего
                                // символа
unsigned char *ring_tail;       // Указатель на следующий необработанный символ
uint32_t        CR_count;        // Счетчик принятых символов ВК (возврат
                                // каретки, т.е. принятых строк)
