#include "Defines.h"
#include "modem.h"
#include <stdlib.h>

#define ctrlZ 0x1A
#define ESC   0x1B

/*================== ������� ����� ������ � ��������� ���������� ============*/
unsigned char modem_ring[RING_SIZE];        // ������� ����� ������
unsigned char *ring_head;       // ��������� �� ����� ��� ������ ����������
                                // �������
unsigned char *ring_tail;       // ��������� �� ��������� �������������� ������
uint8_t        CR_count;        // ������� �������� �������� �� (�������
                                // �������, �.�. �������� �����)
char answer[RING_SIZE];    // ����� ��� ��������� ������� �� ������
char buff[RING_SIZE];      // ����� ��� ������ ���������� � ��������� ����������
char input_buf[RING_SIZE]; // ����� ��� ������ �� TCP/IP
/*====================== ���� ���������� ��������� ������=====================
======================== ����������� � ��������������� �������� ==============*/

#define MIN_DELAY  3000   // �������� � 3 �������



void Dummy(char* ans) {     // �������-�������� ��� ������������� 
    return;                 // ���������������� �������
}


      

/*=========================================================================*/


