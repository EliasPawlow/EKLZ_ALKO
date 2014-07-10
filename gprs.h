// ������� ���������� ������ �� GPRS, ��������� ������ � ��������� ���������
void Init_GPRS(void);
// ����������� ���������
Status On_GPRS(uint32_t timeout);
// ������� ������ ������� � ������ ����� ��� ��������
void Set_Server(char *addr, uint32_t port);
// ������� ������ � ����� ������� �������
void Set_NTP(char *addr, uint32_t port);
// ������� ���������� ����� �������
void Set_GPRS(char *apn, char *user, char *passwd);
// ������� �������� � ��������� �������
Status Connect_Server(uint32_t timeout);
// ����� ������ � ������� ������� time
Status Server_Receive(uint32_t time);
// �������� ������ �� ������
Status Server_Send(char *output, uint32_t len, uint32_t time);
// �������� �������� ���� ������
Status Server_Flush(uint32_t time);