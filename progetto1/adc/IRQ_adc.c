/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "../RIT/RIT.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/
unsigned short AD_current;   
unsigned short AD_last;     /* Last converted value               */
extern objects paddle;
int flag=0;


void ADC_IRQHandler(void) {
	int differenza=0;	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	
	if(flag==0){            //usato solo una volta durante un game, per inizializzare nel modo giusto AD_last e il paddle.b_x
	AD_last=AD_current;
	paddle.b_x=AD_last*190/0xFFF;   //190=240(larghezza display)-50(paddle.width)
	flag=1;
	}
	if(flag>=1){
	if(AD_current<AD_last) differenza=AD_last-AD_current; //controllo sfarfallio
	if(AD_current>AD_last) differenza=AD_current-AD_last; //controllo sfarfallio
  if(AD_current < AD_last && differenza>=60){
		paddle.vel_x=(-1)*differenza*190/0xFFF;	
		AD_last = AD_current;
		} 
	else if (AD_current> AD_last && differenza>=60){
		paddle.vel_x=differenza*190/0xFFF;
		AD_last = AD_current;
		} 
  else{
    paddle.vel_x=0;
    }  
	 }		
  	
}
