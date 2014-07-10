#ifndef __GSM_H
#define __GSM_H
#include "stm32l1xx.h"

#define RING_SIZE  2048

extern unsigned char modem_ring[RING_SIZE];        // ������� ����� ������
extern unsigned char *ring_head;       // ��������� �� ����� ��� ������ ����������
                                 // �������
extern unsigned char *ring_tail;       // ��������� �� ��������� �������������� ������
extern uint32_t        CR_count;        // ������� �������� �������� �� (�������
                                // �������, �.�. �������� �����)

#endif /* __GSM_H */