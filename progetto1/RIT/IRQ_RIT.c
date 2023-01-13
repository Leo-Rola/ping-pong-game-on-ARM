/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../button_EXINT/button.h"
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"
#include "../timer/timer.h"
#include <math.h>


#define RECORD 100          //record iniziale!!
#define thickness 5         //misura minima degli oggetti
#define speed 5 						//misura iniziale dei componenti del vettore velocità ball
enum STATE{
RESET,
GAME,
PAUSE,
GAMEOVER
};
/*
frequenze per timer DAC:
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/

int int0=0;       //gestione pulsanti
int key1=0;
int key2=0;

int score = 0;      //gestione score
int best_score = RECORD;
char testo[10];

walls wall_l,wall_r,wall_top; //gestione oggetti
objects ball,paddle;
int modulo;

extern int flag;     //serve per un paio di ottimizzazioni iniziali (deriva dall'IRQ_adc)
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void RIT_IRQHandler (void)
{
static enum STATE state=RESET;    //stati del game

if(key1 > 1){ 
if(((LPC_GPIO2->FIOPIN & (1<<11)) == 0)&&state==RESET){	/* KEY1 pressed, il game inizia*/				
	switch(key1){
		case 2:
			disable_RIT();				
			GAME_INIZIALIZATION();
			enable_RIT();
			ADC_start_conversion();
			state=GAME;	
			break;
		default:
			break;
	}
	key1++;
}
else {	/* button released */
	key1=0;			
	NVIC_EnableIRQ(EINT1_IRQn);							
	LPC_PINCON->PINSEL4    |= (1 << 22);     
}
}
else { //gestione rimbalzo dei bottoni
if(key1 == 1)
	key1++;
}


if(key2 > 1){ 
if(((LPC_GPIO2->FIOPIN & (1<<12)) == 0)&&(state==GAME||state==PAUSE)){	/* KEY2 pressed, il game va in pausa o riprende */				
	switch(key2){
		case 2:
			if(state==GAME){
				state=PAUSE;
				GUI_Text(100,150,(uint8_t *)"Pause", White, Black);//....
			}		else if(state==PAUSE){
				state=GAME;
				GUI_Text(100,150,(uint8_t *)"Pause", Black, Black);
			}						
			break;
		default:
			break;
	}
	key2++;
}
else {	/* button released */
	key2=0;			
	NVIC_EnableIRQ(EINT2_IRQn);							 
	LPC_PINCON->PINSEL4    |= (1 << 24);     
}
}
else { //gestione rimbalzo dei bottoni
if(key2 == 1)
	key2++;
}


if(int0 > 1){ 
if(((LPC_GPIO2->FIOPIN & (1<<10)) == 0)&&state==GAMEOVER){	/* int0 pressed, in caso di gameover il gioco si resetta*/				
	switch(int0){
		case 2:
				state=RESET;
				flag=0;
				disable_RIT();				
				RESET_DISPLAY();
				enable_RIT();
			break;
		default:
			break;
	}
	int0++;
}
else {	/* button released */
	int0=0;			
	NVIC_EnableIRQ(EINT0_IRQn);							 
	LPC_PINCON->PINSEL4    |= (1 << 20);     
}
}
else { //gestione rimbalzo dei bottoni
if(int0 == 1)
	int0++;
}


if(state==GAME&&flag>=1){          //nucleo del gioco vero e proprio
ADC_start_conversion();            //input
			sprintf(testo,"%d", score);          //draw score (non serve l'undraw)
			GUI_Text(10,150,(uint8_t *) testo, White, Black);
			sprintf(testo, "%d", best_score);
			GUI_Text(200, 10, (uint8_t *) testo, White, Black);

if(flag==2){
 UNDRAW_OLD_OBJECTS();}       //undraw paddle e ball

UPDATE_OBJECTS();           //update paddle e ball
 
DRAW_OBJECTS();             //draw paddle e ball
if(ball.b_y>=290){           //gestione gameover
	 state=GAMEOVER;
	 GUI_Text(100,150,(uint8_t *)"You lose", White, Black);
	 score = 0;
 }	
flag=2;	
}

	
LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}



void RESET_DISPLAY(void){
LCD_Clear(Black);
GUI_Text(0,100,(uint8_t *)"Press KEY1 to start, KEY2 to  pause, INT0 to reset in case  of GAME OVER.                          Good Luck :)", Green, Black);
}

void GAME_INIZIALIZATION(void){
GUI_Text(0,100,(uint8_t *)"Press KEY1 to start, KEY2 to  pause, INT0 to reset in case  of GAME OVER.                          Good Luck :)", Black, Black);
OBJECTS_INITIALIZATION();      //inizializzazione oggetti
DRAW_WALLS();               //draw muri
}

extern void OBJECTS_INITIALIZATION(void){
wall_l.b_x=0;
wall_l.b_y=0;
wall_l.height=278;
wall_l.width=thickness;

wall_r.b_x=235;
wall_r.b_y=0;
wall_r.height=278;
wall_r.width=thickness;

wall_top.b_x=0;
wall_top.b_y=0;
wall_top.height=thickness;
wall_top.width=240;

ball.b_x=230-1;
ball.b_y=158;
ball.height=thickness;
ball.width=thickness;
ball.vel_x=-1*speed;
ball.vel_y=1*speed; 
modulo=sqrt(speed*speed*2);   //serve per mantenere il vettore velocità costante

paddle.b_x=0; 
paddle.b_y=278;
paddle.height=2*thickness;
paddle.width=50;  
paddle.vel_x=0;
paddle.vel_y=0;     //non usata
}

void DRAW_WALLS(void){
uint16_t i;
for(i=0;i<wall_top.height;i++){
LCD_DrawLine(wall_top.b_x,wall_top.b_y+i,wall_top.b_x+wall_top.width,wall_top.b_y+i,Red);
}
for(i=0;i<wall_l.height;i++){
LCD_DrawLine(wall_l.b_x,wall_l.b_y+i,wall_l.b_x+wall_l.width,wall_l.b_y+i,Red);
}
for(i=0;i<wall_r.height;i++){
LCD_DrawLine(wall_r.b_x,wall_r.b_y+i,wall_r.b_x+wall_r.width,wall_r.b_y+i,Red);
}
}




void DRAW_OBJECTS(void){
uint16_t i;
for(i=0;i<ball.height;i++){
LCD_DrawLine(ball.b_x,ball.b_y+i,ball.b_x+ball.width,ball.b_y+i,Green);
}
for(i=0;i<paddle.height;i++){
LCD_DrawLine(paddle.b_x,paddle.b_y+i,paddle.b_x+paddle.width,paddle.b_y+i,Green);
}
}

void UNDRAW_OLD_OBJECTS(void){
uint16_t i;
for(i=0;i<ball.height;i++){
LCD_DrawLine(ball.b_x,ball.b_y+i,ball.b_x+ball.width,ball.b_y+i,Black);
}
for(i=0;i<paddle.height;i++){
LCD_DrawLine(paddle.b_x,paddle.b_y+i,paddle.b_x+paddle.width,paddle.b_y+i,Black);
}
}



void UPDATE_OBJECTS(void){
int diff;

paddle.b_x+=paddle.vel_x;

ball.b_x+=ball.vel_x;
ball.b_y+=ball.vel_y;

diff=ball.b_x-paddle.b_x;
if(ball.b_y<=wall_top.height+4 && ball.vel_y<0){        			  //urto contro muro top

ball.vel_y *= -1;
disable_timer(0);      //suono grave
reset_timer(0);
init_timer(0,2120);
enable_timer(0);

}
else if(ball.b_x<=wall_l.width+4 && ball.vel_x<0 &&ball.b_y<=wall_l.height){        //urto contro muro left

ball.vel_x *= -1;
disable_timer(0);        //suono grave
reset_timer(0);
init_timer(0,2120);
enable_timer(0);

}
else if(ball.b_x>=wall_r.b_x-ball.width-4&& ball.vel_x>0 && ball.b_y<=wall_r.height){       //urto contro muro right

ball.vel_x *= -1;
disable_timer(0);     //suono grave
reset_timer(0);
init_timer(0,2120);
enable_timer(0);

}
else if(ball.b_y>=273&&ball.b_y<=278&&ball.vel_y>0 &&diff>=(-1)*ball.width&&diff<=paddle.width){   		//rimbalzo contro paddle
if(ball.vel_x>0){                                   //proviene da sinistra e va verso destra
	
		 if(diff>=(-1)*ball.width && diff<=10){          
			//rimbalzo a sinistra
							ball.vel_x -=(speed+1); 
							if(ball.vel_x==0) ball.vel_x=-1;
							ball.vel_y =(-1)*sqrt(pow(modulo,2)-pow(ball.vel_x,2));	
							 
		 } else if(diff>10&&diff<=35){          
			//centrale
				ball.vel_y *= -1;
							 
		 } else if(diff>35&&diff<=paddle.width){         
			//rimbalzo a destra
							ball.vel_x +=(speed+1);
							if(ball.vel_x>=modulo) ball.vel_x=(modulo-1);
							ball.vel_y =(-1)*sqrt(pow(modulo,2)-pow(ball.vel_x,2));					 
		 }
}else if(ball.vel_x<0){  			//proviene da destra e va verso sinistra
		 if(diff>=(-1)*ball.width && diff<=10){          
			//rimbalzo a sinistra
							ball.vel_x -=(speed+1);
							if(ball.vel_x<=(-1)*modulo) ball.vel_x=(-1)*(modulo-1);
							ball.vel_y =(-1)*sqrt(pow(modulo,2)-pow(ball.vel_x,2));										 
		 } else if(diff>10&&diff<=35){  
			//centrale
				ball.vel_y *= -1;									 
		 } else if(diff>35&&diff<=paddle.width){        
			//rimbalzo a destra
				ball.vel_x +=(speed+1); 
				if(ball.vel_x==0) ball.vel_x=+1;
				ball.vel_y =(-1)*sqrt(pow(modulo,2)-pow(ball.vel_x,2));				 
		 }
	 
	}
							if(score<=RECORD){      //update score
							score+=5;
							}
							else if(score>RECORD){ 
							score+=10;
							}
							if(score>best_score){
							best_score=score;
							}
				disable_timer(0);        //suono acuto
				reset_timer(0);
				init_timer(0,1062);
				enable_timer(0);									
}


else 
{disable_timer(0);}
}
/******************************************************************************
**                            End Of File
******************************************************************************/
