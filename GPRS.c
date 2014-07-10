#include <lpc_types.h>
#include "Defines.h"
#include "gprs.h"
#include "Modem.h"

// Установка параметров точки доступа GPRS
void Set_GPRS(char *apn, char *user, char *passwd) {
  put_STR(apn, gprs_apn);
  put_STR(user, gprs_user);
  put_STR(passwd, gprs_passwd);
}

void Set_NTP(char *addr, uint32_t port) {
  put_STR(addr, qntp_server);
  qntp_port = port;
}

// Задание параметров работы GPRS
void Init_GPRS(void) {
    modem_QISTAT();
    // TODO: Может, надо ввести таймаут на операцию
    while (qistat != IP_INITIAL) {
      modem_QIDEACT();
      modem_QISTAT();
    }
  modem_QIFGCNT(OFF); // Установим нулевой контекст
  modem_QICSGP(GPRS); // Установим подключение по GPRS (параметры точки доступа
                      // должны быть заполнены)  
  modem_QIMUX(OFF);   // Запретим множественные коннекты
  modem_QIMODE(OFF);  // Непрозрачный режим обмена
  modem_QIDNSIP(OFF); // Запрет работы DNS
}
  
// установка адреса и номера порта сервера, куда коннектимся
void Set_Server(char *addr, uint32_t port) {
  put_STR(addr, qi_remoteip);
  qi_remoteport = port;
}
// регистрация в Интернет посредством GPRS
Status On_GPRS(uint32_t timeout) {
  Timer1_Start(timeout);
  while(!Timer1_Status()) {
    modem_QISTAT();
    switch (qistat) {
    case IP_INITIAL:
      modem_QIREGAPP(OFF);   // регистрируем подключение GPRS
      break;
    case IP_START:
      modem_QIACT();      // регистрируемся в сети
    case IP_CONFIG:
    case IP_GPRSACT:
      modem_QILOCIP();    // Запрашиваем локальный IP адрес
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
// попытка коннекта к серверу с указанным таймаутом (в секундах)
Status Connect_Server(uint32_t timeout) {
  
  input_len = 0;
  
  modem_QISTAT();
  modem_QISRVC(CS_STATE);
  if(qisrvc == SERVER) modem_QISRVC(CLIENT);
  modem_QINDI(ON); // Включаем оповещение о приходе очередной порции данных
  modem_QIOPEN("TCP",qi_remoteip,qi_remoteport,timeout);
  if (connect == 1) return SUCCESS;
  return ERROR;
}
// Ожидание прихода данных в течении времени time. Если данные пришли - SUCCESS
Status Server_Receive(uint32_t time) {

  uint32_t len = 0;

  Timer1_Start(time);
  while(!Timer1_Status()) {
      while (GetAnswer(answer)) {
        Parse_Ans(answer);
      }
      if (!connect) return ERROR; // Пропал коннект, ничего не делаем
      if (qirdi) {
        do {
          len = input_len;
          modem_QIRD(RING_SIZE - input_len); // Чтобы случайно из буфера не выйти
        } while (len != input_len);
        if (input_len > 5) break;
      }
    }
    Timer1_Stop();
  if (len != input_len) return ERROR;
  return SUCCESS;
}

Status Server_Send(char *output, uint32_t len, uint32_t time) {
  // Вывод данных из буфера output длиной len за время time
  uint32_t count = 0;
  uint32_t pktlen;

  if (!connect) return ERROR; // Пропал коннект, ничего не делаем

  modem_QISDE(OFF);   // Отключим эхо при выводе
  Timer1_Start(time);
  while(!Timer1_Status() && len) {
    if (len > 1460) {
      pktlen = 1460;
    } else pktlen = len;
    modem_QISEND(output, pktlen);
    if (!connect) return ERROR;   // В процессе передачи связь разорвалась
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
// Ожидание очистки выходного буфера
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