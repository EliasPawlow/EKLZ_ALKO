#include <lpc_types.h>
#include "Defines.h"
#include "gprs.h"
#include "Modem.h"

// ��������� ���������� ����� ������� GPRS
void Set_GPRS(char *apn, char *user, char *passwd) {
  put_STR(apn, gprs_apn);
  put_STR(user, gprs_user);
  put_STR(passwd, gprs_passwd);
}

void Set_NTP(char *addr, uint32_t port) {
  put_STR(addr, qntp_server);
  qntp_port = port;
}

// ������� ���������� ������ GPRS
void Init_GPRS(void) {
    modem_QISTAT();
    // TODO: �����, ���� ������ ������� �� ��������
    while (qistat != IP_INITIAL) {
      modem_QIDEACT();
      modem_QISTAT();
    }
  modem_QIFGCNT(OFF); // ��������� ������� ��������
  modem_QICSGP(GPRS); // ��������� ����������� �� GPRS (��������� ����� �������
                      // ������ ���� ���������)  
  modem_QIMUX(OFF);   // �������� ������������� ��������
  modem_QIMODE(OFF);  // ������������ ����� ������
  modem_QIDNSIP(OFF); // ������ ������ DNS
}
  
// ��������� ������ � ������ ����� �������, ���� �����������
void Set_Server(char *addr, uint32_t port) {
  put_STR(addr, qi_remoteip);
  qi_remoteport = port;
}
// ����������� � �������� ����������� GPRS
Status On_GPRS(uint32_t timeout) {
  Timer1_Start(timeout);
  while(!Timer1_Status()) {
    modem_QISTAT();
    switch (qistat) {
    case IP_INITIAL:
      modem_QIREGAPP(OFF);   // ������������ ����������� GPRS
      break;
    case IP_START:
      modem_QIACT();      // �������������� � ����
    case IP_CONFIG:
    case IP_GPRSACT:
      modem_QILOCIP();    // ����������� ��������� IP �����
      break;
    case IP_STATUS:
    case IP_CONNECTING:
    case IP_CLOSE:
    case CONNECT_OK:
         Timer1_Stop();
         return SUCCESS;
    case IP_IND:
      break;
    case PDP_DEACT:
    default:
      modem_QIDEACT();
    }
  } // End while
  Timer1_Stop();
  return ERROR;
}
// ������� �������� � ������� � ��������� ��������� (� ��������)
Status Connect_Server(uint32_t timeout) {
  
  input_len = 0;
  
  modem_QISTAT();
  modem_QISRVC(CS_STATE);
  if(qisrvc == SERVER) modem_QISRVC(CLIENT);
  modem_QINDI(ON); // �������� ���������� � ������� ��������� ������ ������
  modem_QIOPEN("TCP",qi_remoteip,qi_remoteport,timeout);
  if (connect == 1) return SUCCESS;
  return ERROR;
}
// �������� ������� ������ � ������� ������� time. ���� ������ ������ - SUCCESS
Status Server_Receive(uint32_t time) {

  uint32_t len = 0;

  Timer1_Start(time);
  while(!Timer1_Status()) {
      while (GetAnswer(answer)) {
        Parse_Ans(answer);
      }
      if (!connect) return ERROR; // ������ �������, ������ �� ������
      if (qirdi) {
        do {
          len = input_len;
          modem_QIRD(RING_SIZE - input_len); // ����� �������� �� ������ �� �����
        } while (len != input_len);
        if (input_len > 5) break;
      }
    }
    Timer1_Stop();
  if (len != input_len) return ERROR;
  return SUCCESS;
}

Status Server_Send(char *output, uint32_t len, uint32_t time) {
  // ����� ������ �� ������ output ������ len �� ����� time
  uint32_t count = 0;
  uint32_t pktlen;

  if (!connect) return ERROR; // ������ �������, ������ �� ������

  modem_QISDE(OFF);   // �������� ��� ��� ������
  Timer1_Start(time);
  while(!Timer1_Status() && len) {
    if (len > 1460) {
      pktlen = 1460;
    } else pktlen = len;
    modem_QISEND(output, pktlen);
    if (!connect) return ERROR;   // � �������� �������� ����� �����������
    if (mdm_err) break;
    if (send_ok == 1) {
      len -= pktlen;
      output += pktlen;
    }
  }
  Timer1_Stop();
  if (len) return ERROR;
  
  return SUCCESS;
}
// �������� ������� ��������� ������
Status Server_Flush(uint32_t time) {
  Timer1_Start(time);
  while (!Timer1_Status()) {
    modem_QISACK(0);
    if (qi_sent == qi_acked) break;
  }
  Timer1_Stop();
  if (qi_sent == qi_acked) return SUCCESS;
  return ERROR;
}