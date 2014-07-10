#ifndef __GSM_H
#define __GSM_H
#include "stm32l1xx.h"

#define RING_SIZE  2048

extern unsigned char modem_ring[RING_SIZE];        // Входной буфер модема
extern unsigned char *ring_head;       // Указатель на место для приема следующего
                                 // символа
extern unsigned char *ring_tail;       // Указатель на следующий необработанный символ
extern uint32_t        CR_count;        // Счетчик принятых символов ВК (возврат
                                // каретки, т.е. принятых строк)

#endif /* __GSM_H */