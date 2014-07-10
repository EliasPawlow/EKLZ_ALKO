
#define MIN_DELAY  3000   // �������� � 3 �������
#define RING_SIZE 	        2048     //������ ��������� ������ �� ������



typedef enum {
  S2400 = 0,
  S4800,
  S9600,
  S14400
} RATES;


/*================== ������� ����� ������ � ��������� ���������� ============*/
extern unsigned char modem_ring[RING_SIZE];        // ������� ����� ������
extern unsigned char *ring_head;       // ��������� �� ����� ��� ������ ����������
                                // �������
extern unsigned char *ring_tail;       // ��������� �� ��������� �������������� ������
extern uint8_t        CR_count;        // ������� �������� �������� �� (�������
                                // �������, �.�. �������� �����)

