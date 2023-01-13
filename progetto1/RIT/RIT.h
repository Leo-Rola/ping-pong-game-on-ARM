/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_RIT, funct_RIT, IRQ_RIT .c files
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __RIT_H
#define __RIT_H

#include "../adc/adc.h"

typedef struct WALLS{       //definizione oggetto muri
	uint16_t b_x;
	uint16_t b_y;
	uint16_t width;
	uint16_t height;
}walls;

typedef struct OBJECTS{         //definizione oggetto paddle o ball
	uint16_t b_x;
	uint16_t b_y;
	int vel_x;
	int vel_y;
	uint16_t width;
	uint16_t height;
}objects;

/* init_RIT.c */
extern uint32_t init_RIT( uint32_t RITInterval );
extern void enable_RIT( void );
extern void disable_RIT( void );
extern void reset_RIT( void );
/* IRQ_RIT.c */
extern void RIT_IRQHandler (void);
/* Custom functions  */
extern void RESET_DISPLAY(void);
extern void GAME_INIZIALIZATION(void);
extern void OBJECTS_INITIALIZATION(void);
extern void DRAW_WALLS(void);
extern void DRAW_OBJECTS(void);
extern void UNDRAW_OLD_OBJECTS(void);
extern void UPDATE_OBJECTS(void);

#endif /* end __RIT_H */


/*****************************************************************************
**                            End Of File
******************************************************************************/
