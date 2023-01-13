/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT and joystick buttons
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
 
int main(){
	SystemInit();                         /* SYSTEM Initialization          */
	LCD_Initialization();                 /* LCD Initialization            */
	BUTTON_init();												/* BUTTON Initialization              */
  RESET_DISPLAY();                    	/* custom function dichiarata nell'IRQ_RIT       */
	
	init_RIT(0x004C4B40);									/* 0x004C4B40-->RIT Initialization 50 msec       	*/
	                                      /* 0x002625A0-->RIT Initialization 25 msec --> scelta per avere un fps maggiore di 30
                                        anche se minore di 60, e il gioco funziona ancora in tutti i suoi aspetti	*/
	                                      /* 0x003D0900-->RIT Initialization 40 msec        */
	                                      /* 0x0016E360-->RIT Initialization 15 msec        */
	                                      /* 0x000F4240-->RIT Initialization 10 msec        */
	
	enable_RIT();													/* RIT enabled  		*/
	ADC_init();                           /* ADC Initialization       */
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);				

	while(1){
		__ASM("wfi");
	}
}
