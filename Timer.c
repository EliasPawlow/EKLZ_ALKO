#include "Defines.h"

TIM_TIMERCFG_Type TIM0_Config;              // ������������ �������
TIM_MATCHCFG_Type TIM0_Match;               // ��������� ���������
TIM_TIMERCFG_Type TIM1_Config;              // ������������ �������
TIM_MATCHCFG_Type TIM1_Match;               // ��������� ���������

static volatile uint32_t msTicks = 0;           // counts 1ms timeTicks
/* ============== ��������� ������� ===============*/
void SysTick_Handler(void);

//======================================================================
void SysTick_Handler(void)  // ���������� ���������� �� SysTick
{
        msTicks++;              // ��������� �������� �������
        if (msTicks >= time_out) 
        {  // ���� ������� �����
        if(!UART_RCV_COUNT) Timer_Start();
	else{ 
             	UART_DONE=0xff;
	    }

        }
}

void Timer_Init(void) {
  
        SystemCoreClockUpdate(); // ������� ������� ������� CPU !
  //�������������� System Tick �� �������� 1ms
	SYSTICK_InternalInit(1);
  // ��� ������� ��������� ������ �������
        SYSTICK_Cmd(DISABLE);
  // ��������� ���������� �� �������
       	SYSTICK_IntCmd(ENABLE);
  // �������� ������� �����������
        msTicks = 0;
}
void Timer_Restart(void) {
  msTicks = 0;
}

void Timer_Start(void) {
  msTicks = 0;
  SYSTICK_Cmd(ENABLE);
}

void Timer_Stop(void) {
  SYSTICK_Cmd(DISABLE);
   msTicks = 0;
}
/*============== ������ 0, �������� ������ � ������ � ������� =============*/
void Init_Timer0 (void)  // ��������� ������������� ������� �������� ������
                         // � ����������������� � �������
{
      // �������� ����� � �������������
      TIM0_Config.PrescaleOption = TIM_PRESCALE_USVAL; 
      // ���������� ������� �������� 1 ����.
      TIM0_Config.PrescaleValue	= 1000;
      // ���������� ����� 0, MR0
      TIM0_Match.MatchChannel = 0;
      // �������� ����������, ���� MR0 �������� �� ��������� ������� TC
      TIM0_Match.IntOnMatch   = TRUE;
      //��������� ����� �� MR0: TIMER ���������, ���� MR0 �������� � ���
      TIM0_Match.ResetOnMatch = TRUE;
      //���������� ������ ��� ���������� MR0
      TIM0_Match.StopOnMatch  = TRUE;
      //������� ������� �� ������� �� ���������
      TIM0_Match.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
      // ��������� �������� ������������ (1000 * 1 mS = 1 s)
      TIM0_Match.MatchValue   = 1000;

      TIM_Init(TIMER0, TIM_TIMER_MODE,&TIM0_Config);
      TIM_ConfigMatch(TIMER0,&TIM0_Match);
//      TIM_Cmd(LPC_TIM0,ENABLE);
}

void Timer0_Start(uint32_t Delay)
{
  TIM0_Match.MatchValue   = Delay;
  TIM_ConfigMatch(TIMER0,&TIM0_Match);
  TIM_ResetCounter(TIMER0);
  TIM_Cmd(TIMER0,ENABLE);
}

void Timer0_Stop(void)
{
      TIM_ClearIntPending(TIMER0,TIM_MR0_INT);
      TIM_Cmd(TIMER0,DISABLE);
}

Status Timer0_Status(void)
{
  if (TIM_GetIntStatus(TIMER0,TIM_MR0_INT))
  {
    return SUCCESS;
  } else {
    return ERROR;
  }
}

/*============== ������ 1, ���� ��� ������� �������� =============*/
void Init_Timer1 (void)  // ��������� ������������� ������� �������� ������
                         // � ����������������� � �������
{
      // �������� ����� � �������������
      TIM1_Config.PrescaleOption = TIM_PRESCALE_USVAL; 
      // ���������� ������� �������� 0.1 ���.
      TIM1_Config.PrescaleValue	= 100000;
      // ���������� ����� 0, MR0
      TIM1_Match.MatchChannel = 1;
      // �������� ����������, ���� MR0 �������� �� ��������� ������� TC
      TIM1_Match.IntOnMatch   = TRUE;
      //��������� ����� �� MR0: TIMER ���������, ���� MR0 �������� � ���
      TIM1_Match.ResetOnMatch = TRUE;
      //���������� ������ ��� ���������� MR0
      TIM1_Match.StopOnMatch  = TRUE;
      //������� ������� �� ������� �� ���������
      TIM1_Match.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
      // ��������� �������� ������������ (1000 * 0.1 S = 100 s)
      TIM1_Match.MatchValue   = 1000;

      TIM_Init(TIMER1, TIM_TIMER_MODE,&TIM1_Config);
      TIM_ConfigMatch(TIMER1,&TIM1_Match);
//      TIM_Cmd(LPC_TIM0,ENABLE);
}

void Timer1_Start(uint32_t Delay) // �������� � ��� * 10
{
  TIM1_Match.MatchValue   = Delay;
  TIM_ConfigMatch(TIMER1,&TIM1_Match);
  TIM_ResetCounter(TIMER1);
  TIM_Cmd(TIMER1,ENABLE);
}

void Timer1_Stop(void)
{
      TIM_ClearIntPending(TIMER1,TIM_MR1_INT);
      TIM_Cmd(TIMER1,DISABLE);
}

Status Timer1_Status(void)
{
  if (TIM_GetIntStatus(TIMER1,TIM_MR1_INT))
  {
    return SUCCESS;
  } else {
    return ERROR;
  }
}


//========= ������������ ������������ ��������, �������� � ms==================
void Wait(uint32_t Delay)
{
  Timer0_Start(Delay);
  while(!Timer0_Status());
  Timer0_Stop();
}