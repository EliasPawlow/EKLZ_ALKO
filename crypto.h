
typedef union {
  char  addr_byte[4];
  uint32_t  address;
} ARC_address;

typedef union {
  uint32_t muza_mac32;
  char    muza_mac[4];     // ����� ��� ������ MAC
} MAC_num;



extern MAC_num MAC;
extern uint8_t muza_stat;
extern char    muza_buff[256];  // ����� ��� ������ � �����������������
extern char    muza_mac[3];     // ����� ��� ������ MAC

/* ������ ��������� ��. ���� ERROR, �� ������, ����� muza_stat 
  ���������� delay ������ � ������� ������� ����� �������� ���������� */
Status MUZA_Status_OLD(uint32_t delay);
Status MUZA_Status(uint32_t delay);
/* �������� ������. ���� ����� �������� ��� ����� � ���������� ��������� delay,
�� ������ */
Status MUZA_erase(void);
Status MUZA_TransmitMSG( uint8_t len_msg_in, unsigned char *ptr);
Status MUZA_ReceiveMSG(uint8_t len_msg_out, unsigned char *ptr);
Status MUZA_Save(uint8_t len, char *ptr);
Status MUZA_Dump(uint32_t len, char *addr, char *ptr);
Status MUZA_Ver_Software(uint8_t len, char *ptr);
Status MUZA_IO_Control(uint32_t delay);
Status MUZA_one(void);
Status MUZA_two(void);
Status MUZA_Doc(uint8_t *count, unsigned char *ptr);
Status MUZA_check(char *ptr1, char *ptr2);
Status MUZA_num(uint32_t delay); // ������ ��������� ������
Status MUZA_Test(uint32_t delay); // �������� ������������
Status MUZA_Act(uint8_t len, char *ptr); // �����������
Status MUZA_Close(uint8_t len, char *ptr); // �������� ������
Status MUZA_FPO(uint32_t delay);   // �������� ���

          //    #define CT 0x2000       //�� 64 �����
           //   #define CU 0x0C00       //�� 64 �����
#define CT 0x0800       //�� 256 ����
#define CU 0x0200       //�� 256 ����