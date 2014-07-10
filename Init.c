#include "Defines.h"
/*                    Публичные функции                                      */

//==== Начальная инициализация портов ввода-вывода ============================
void Init_GPIO(void) {
  
// Порт P2.8 - кнопка PWR для модема, сделаем выходом и запишем 0
// Порт P2.4 - вход статуса модема "1" - включен, "0" - выключен
  PINSEL_CFG_Type PinCfg;

  PinCfg.Funcnum = 0;                        // Обычный вход-выход (GPIO)
  PinCfg.Portnum = PWR_PORT;                 // Порт 2
  
  PinCfg.Pinnum =  PWR_PIN;                 // Пин 8
        PINSEL_ConfigPin(&PinCfg);           // Настройка P2.8 (PWR)
        GPIO_SetDir(PWR_PORT, (1<<PWR_PIN), 1);    // Сделаем его выходом
        GPIO_ClearValue(PWR_PORT, (1<<PWR_PIN));   // И сразу установим в "0"
//        PinCfg.Portnum = STATUS_PORT;                 // Порт 2
        PinCfg.Pinnum =  STATUS_PIN;
        PINSEL_ConfigPin(&PinCfg);            // Настройка P2.4 (STATUS)
        GPIO_SetDir(STATUS_PORT, (1<<STATUS_PIN), 0);  // Сделаем его входом
// настройка выводов для I2C1
        PinCfg.Funcnum = 3;                 // Для P0.0 P0.1 назначить SDA, SCL
        PinCfg.Pinnum = 0;
        PinCfg.Portnum = 0;
        PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
        PinCfg.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 1;
        PINSEL_ConfigPin(&PinCfg);
    
}
//==== Инициализация модуля АЦП для измерения напряжения питания ==============
void Init_ADC(void) {
}
//==== Инициализация модуля I2C для обмена с кассой ===========================
void Init_I2C2(void) {
}
//==== Инициализация модуля USB ===============================================
void Init_USB(void) {
}
//=============================================================================