// Задание параметров работы по GPRS, допустимо только в начальном состоянии
void Init_GPRS(void);
// Регистрация контекста
Status On_GPRS(uint32_t timeout);
// Задание адреса сервера и номера порта для коннекта
void Set_Server(char *addr, uint32_t port);
// Задание адреса и порта сервера времени
void Set_NTP(char *addr, uint32_t port);
// Задание параметров точки доступа
void Set_GPRS(char *apn, char *user, char *passwd);
// Попытка коннекта к заданному серверу
Status Connect_Server(uint32_t timeout);
// Прием данных в течении времени time
Status Server_Receive(uint32_t time);
// Передача данных на сервер
Status Server_Send(char *output, uint32_t len, uint32_t time);
// Ожидание отправки всех данных
Status Server_Flush(uint32_t time);