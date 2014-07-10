#include "Defines.h"

TIM_TIMERCFG_Type TIM0_Config;              // Конфигурация таймера
TIM_MATCHCFG_Type TIM0_Match;               // Параметры интервала
TIM_TIMERCFG_Type TIM1_Config;              // Конфигурация таймера
TIM_MATCHCFG_Type TIM1_Match;               // Параметры интервала

static volatile uint32_t msTicks = 0;           // counts 1ms timeTicks
/* ============== Локальные функции ===============*/
void SysTick_Handler(void);

//======================================================================
void SysTick_Handler(void)  // Обработчик прерываний от SysTick
{
        msTicks++;              // инкремент счётчика времени
        if (msTicks >= time_out) 
        {  // Если таймаут истек
        if(!UART_RCV_COUNT) Timer_Start();
	else{ 
             	UART_DONE=0xff;
	    }

        }
}

void Timer_Init(void) {
  
        SystemCoreClockUpdate(); // Получим текущую частоту CPU !
  //Инициализируем System Tick на интервал 1ms
	SYSTICK_InternalInit(1);
  // Для порядка запрещаем работу таймера
        SYSTICK_Cmd(DISABLE);
  // Разрешаем прерывания от таймера
       	SYSTICK_IntCmd(ENABLE);
  // Обнуляем счетчик миллисекунд
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
/*============== Таймер 0, таймауты обмена с крипто и модемом =============*/
void Init_Timer0 (void)  // Начальная инициализация таймера таймаута обмена
                         // с криптопроцессором и модемом
{
      // Задавать будем в микросекундах
      TIM0_Config.PrescaleOption = TIM_PRESCALE_USVAL; 
      // Изначально зададим интервал 1 мсек.
      TIM0_Config.PrescaleValue	= 1000;
      // Используем канал 0, MR0
      TIM0_Match.MatchChannel = 0;
      // Разрешим прерывание, если MR0 совпадет со счетчиком таймера TC
      TIM0_Match.IntOnMatch   = TRUE;
      //Разрешить сброс по MR0: TIMER сбросится, если MR0 совпадет с ним
      TIM0_Match.ResetOnMatch = TRUE;
      //Остановить таймер при достижении MR0
      TIM0_Match.StopOnMatch  = TRUE;
      //Никаким выходом по таймеру не управляем
      TIM0_Match.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
      // Установим интервал срабатывания (1000 * 1 mS = 1 s)
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

/*============== Таймер 1, часы для длинных операций =============*/
void Init_Timer1 (void)  // Начальная инициализация таймера таймаута обмена
                         // с криптопроцессором и модемом
{
      // Задавать будем в микросекундах
      TIM1_Config.PrescaleOption = TIM_PRESCALE_USVAL; 
      // Изначально зададим интервал 0.1 сек.
      TIM1_Config.PrescaleValue	= 100000;
      // Используем канал 0, MR0
      TIM1_Match.MatchChannel = 1;
      // Разрешим прерывание, если MR0 совпадет со счетчиком таймера TC
      TIM1_Match.IntOnMatch   = TRUE;
      //Разрешить сброс по MR0: TIMER сбросится, если MR0 совпадет с ним
      TIM1_Match.ResetOnMatch = TRUE;
      //Остановить таймер при достижении MR0
      TIM1_Match.StopOnMatch  = TRUE;
      //Никаким выходом по таймеру не управляем
      TIM1_Match.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
      // Установим интервал срабатывания (1000 * 0.1 S = 100 s)
      TIM1_Match.MatchValue   = 1000;

      TIM_Init(TIMER1, TIM_TIMER_MODE,&TIM1_Config);
      TIM_ConfigMatch(TIMER1,&TIM1_Match);
//      TIM_Cmd(LPC_TIM0,ENABLE);
}

void Timer1_Start(uint32_t Delay) // Задержка в сек * 10
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


//========= Подпрограмма формирования задержки, заданной в ms==================
void Wait(uint32_t Delay)
{
  Timer0_Start(Delay);
  while(!Timer0_Status());
  Timer0_Stop();
}