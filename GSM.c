#include "gsm.h"

unsigned char modem_ring[RING_SIZE];        // ������� ����� ������
unsigned char *ring_head;       // ��������� �� ����� ��� ������ ����������
                                // �������
unsigned char *ring_tail;       // ��������� �� ��������� �������������� ������
uint32_t        CR_count;        // ������� �������� �������� �� (�������
                                // �������, �.�. �������� �����)
