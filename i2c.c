#include "Defines.h"
#include "lpc17xx_i2c.h"
//===============================================================
void I2C0_IRQHandler(void)
{
unsigned char StatValue_0; // Сюда будем класть текущее состояние I2C
unsigned char SlaveRcv;    // Сюда принятый по I2C байт
static uint32_t buf_count;       // Счетчик принятых байтов

  Channel = I2C;
  StatValue_0 =( LPC_I2C0 -> I2STAT & I2C_STAT_CODE_BITMASK);
  if(StatValue_0 == I2C_STAT_CODE_BITMASK) { 
    LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC; 
    return;
  }
//---------------------------------------------	
  switch ( StatValue_0 )
  {

       case I2C_STAT_CODE_BITMASK:  // 0xF8 - неопределенное состояние
		 LPC_I2C0->I2CONSET = I2C_I2CONSET_AA;
		 LPC_I2C0->I2CONCLR = I2C_I2CONCLR_SIC;
		 break;
//-----------------------                 
      case I2C_I2STAT_S_RX_SLAW_ACK :    // 0x60 Получен адрес, отправлен ACK                                               	
      case I2C_I2STAT_S_RX_GENCALL_ACK : // 0x70 Получен широковещательный адрес,
    //  отправлен ACK. Это начало приема команды
    //  Установить в 0 счетчик приема
                buf_count = 0;
                LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;		
	        LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;
                break;
//-----------------------
	 case I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK : // 0x80 Прием байта по своему адресу                                                
         case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK : // 0x90 прием байта в 
         // широковещательном режиме          
         // Читаем байт из регистра приема
           SlaveRcv = LPC_I2C0 -> I2DAT;		
                data_rd_UART[buf_count]=SlaveRcv;
		buf_count++;

                if(buf_count == data_rd_UART[1]+3)// TODO: Проверить !
                {
                  LPC_I2C0 -> I2CONCLR = (I2C_I2CONCLR_SIC | I2C_I2CONCLR_AAC);
                  break;
                }
	   
                 LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;
                 LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;
		 break;
//-----------------------
         case I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK : // 0x88                                               
           // Байт принят по своему адресу, но ACK не отправлен
	 case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK: // 0x98
           // Байт принят по широковещательному адресу, но ACK не отправлен
                 LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;	               
                 SlaveRcv = LPC_I2C0 -> I2DAT;		            
                 break;
//-----------------------			
          case I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX : // 0xA0
    // Состояние STOP или повторяющийся START в режиме SLAVE
		 LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;                       
		 LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;	
		 break;
//---------------------------------------------------------------------	
	  case I2C_I2STAT_S_TX_SLAR_ACK :  // 0xA8                      
            // Запрос передачи в режиме SLAVE. Был получен свой
            // адрес с битом запроса данных. Передаем первый байт буфера
                buf_count = 1;
                LPC_I2C0 -> I2DAT = data_wr_UART[0];
                LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;                        
		LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;	                                
                break;
            
            
          case I2C_I2STAT_S_TX_DAT_ACK : // 0xB8
            // Передача в режиме SLAVE, предыдущий байт передан, получен ACK
            
            if(buf_count < data_wr_UART[1]+3) // TODO: Проверить !
            {
              // Передаем очередной байт, увеличиваем счетчик
                LPC_I2C0 -> I2DAT = data_wr_UART[buf_count];
		buf_count++;		          
                LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;                        
	        LPC_I2C0 -> I2CONCLR = I2C_I2CONCLR_SIC;
                break;
            } else  {
            // Здесь передаем последний байт и убираем ACK, чтобы мастер закончил
            // прием !
                  LPC_I2C0 -> I2DAT = data_wr_UART[buf_count];
		  LPC_I2C0 -> I2CONCLR = ( I2C_I2CONCLR_SIC | I2C_I2CONCLR_AAC);
                break;
            }                            
            
//-------------------------------------------------------------------------                
            case I2C_I2STAT_S_TX_DAT_NACK : // 0xC0
              // Данные переданы, получен NACK
		 LPC_I2C0 -> I2CONSET = I2C_I2CONSET_AA;                       
                 LPC_I2C0 -> I2CONCLR = (I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC);
		 break;
              
//-----------------------
            default: 
              
                 LPC_I2C0 -> I2CONCLR = (I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC);
                  break;
          }
}
//unsigned int I2CInit_0( unsigned int I2cMode_0 )
void I2CInit_0(void)
{

   LPC_PINCON -> PINSEL1 |= 0x01400000; //Это для LPC100 I2C0
//   LPC_PINCON -> PINSEL0 |= 0x0000000F; // Это для LPC80 I2C1
   
   
  /*--- Clear flags ---*/
  LPC_I2C0 -> I2CONCLR = (I2C_I2CONCLR_AAC | I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC | I2C_I2CONCLR_I2ENC);

  /*--- Reset registers ---*/
  //LPC_I2C0 -> I2SCLL   = I2SCLL_SCLL;
  //LPC_I2C0 -> I2SCLH   = I2SCLH_SCLH;


	LPC_I2C0 -> I2ADR0 = 0x02;
	LPC_I2C0 -> I2MASK0 =0x07;

  /* Install interrupt handler */	

    NVIC_EnableIRQ(I2C0_IRQn);

    LPC_I2C0 -> I2CONSET = (I2C_I2CONSET_I2EN |I2C_I2CONSET_AA);
  return;
}
//===========================================================================
