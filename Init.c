#include "Defines.h"
/*                    ��������� �������                                      */

//==== ��������� ������������� ������ �����-������ ============================
void Init_GPIO(void) {
  
// ���� P2.8 - ������ PWR ��� ������, ������� ������� � ������� 0
// ���� P2.4 - ���� ������� ������ "1" - �������, "0" - ��������
  PINSEL_CFG_Type PinCfg;

  PinCfg.Funcnum = 0;                        // ������� ����-����� (GPIO)
  PinCfg.Portnum = PWR_PORT;                 // ���� 2
  
  PinCfg.Pinnum =  PWR_PIN;                 // ��� 8
        PINSEL_ConfigPin(&PinCfg);           // ��������� P2.8 (PWR)
        GPIO_SetDir(PWR_PORT, (1<<PWR_PIN), 1);    // ������� ��� �������
        GPIO_ClearValue(PWR_PORT, (1<<PWR_PIN));   // � ����� ��������� � "0"
//        PinCfg.Portnum = STATUS_PORT;                 // ���� 2
        PinCfg.Pinnum =  STATUS_PIN;
        PINSEL_ConfigPin(&PinCfg);            // ��������� P2.4 (STATUS)
        GPIO_SetDir(STATUS_PORT, (1<<STATUS_PIN), 0);  // ������� ��� ������
// ��������� ������� ��� I2C1
        PinCfg.Funcnum = 3;                 // ��� P0.0 P0.1 ��������� SDA, SCL
        PinCfg.Pinnum = 0;
        PinCfg.Portnum = 0;
        PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
        PinCfg.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 1;
        PINSEL_ConfigPin(&PinCfg);
    
}
//==== ������������� ������ ��� ��� ��������� ���������� ������� ==============
void Init_ADC(void) {
}
//==== ������������� ������ I2C ��� ������ � ������ ===========================
void Init_I2C2(void) {
}
//==== ������������� ������ USB ===============================================
void Init_USB(void) {
}
//=============================================================================