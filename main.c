/*
 * FinalProject.c
 *
 * Created: 11/14/2025 11:03:34 PM
 * Author : ajb21b
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ACU_SH1106_gfxlib.h"
#include "fonts/FreeMono12pt7b.h"
#include "tetris.h"
//#define DELAY_COUNT	40000
struct Tetris t;

volatile uint16_t adc_res;
volatile uint8_t drop;
volatile uint8_t update;
volatile InputType joystickPos;
volatile uint8_t adc_channel;


void initJoystick(){
	PORTA.DIRCLR = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm;
	
	//Not using the ADC any more. Sick and tired of the resets.
	/*
	ADC0.CTRLC |= ADC_PRESC_DIV64_gc | ADC_REFSEL_4096MV_gc;
	ADC0.INTCTRL |= ADC_RESRDY_bm;
	ADC0.MUXPOS |= ADC_MUXPOS_AIN1_gc;
	adc_channel = 0;
	ADC0.COMMAND |= ADC_MODE_SINGLE_12BIT_gc;
	ADC0.CTRLA |= ADC_ENABLE_bm | ADC_RUNSTDBY_bm;
	*/
}

void initTCA0(){
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_RUNSTDBY_bm;
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = 500;
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void initButton(){
	PORTC.DIRCLR = PIN4_bm;
	PORTC.PIN4CTRL |= PORT_PULLUPEN_bm;
}


void fallEvent(){
    if(!tetris_moveDown(&t)){
        tetris_addBlock(&t);
        tetris_checkClear(&t);
        tetris_newBlock(&t);
    }
}


int main(void)
{	
	drop = 0;
	update = 0;
	initSPI();
	initScreen();
	//setFont(&FreeMono12pt7b);
	setTextColor(COLOR_BLACK, COLOR_WHITE);

	tetris_init(&t);

	initJoystick();
	initButton();
	initTCA0();
	
	//invertDisplay(1);
	clearDisplay();
	tetris_draw(&t);
	showScreen();
	sei();
    while(1)
    {
		if(t.gameOver){
			tetris_init(&t);
			drop = 0;
			update = 0;
			continue;
		}
		if(drop == 3){
			drop = 0;
			fallEvent();
			clearDisplay();
			tetris_draw(&t);
			showScreen();
		}
		
		if(update){
			update = 0;
			clearDisplay();
			tetris_draw(&t);
			showScreen();
			if(PORTA.IN & PIN2_bm){
				tetris_move(&t, Right);
			}
			else if(PORTA.IN & PIN1_bm){
				tetris_move(&t, Left);
			}
			else if(PORTA.IN & PIN3_bm){
				tetris_move(&t, Rotate);
			}
			else if(PORTA.IN & PIN4_bm){
				tetris_move(&t, Down);
			}
			if(!(PORTC.IN & PIN4_bm)){
				tetris_swap(&t);
			}
		}
    }
}

//Not using the ADC any more. Sick and tired of the resets.
/*
ISR(ADC0_RESRDY_vect){
	adc_res = ADC0.RESULT;
	if(adc_channel == 0){
		if(adc_res < 2700){
			//circle.dx = 1;
			joystickPos = Right;
			
		}
		else if(adc_res > 3800){
			//circle.dx = -1;
			joystickPos = Left;
		}
		else{
			//circle.dx = 0;
			//joystickPos = Neutral;
		}
		ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
		adc_channel = 1;
	}
	else if(adc_channel == 1){
		if(adc_res < 2000){
			//circle.dy = 1;
			joystickPos = Rotate;
		}
		else if(adc_res > 3000){
			//circle.dy = -1;
			joystickPos = Down;
		}
		else{
			//circle.dy = 0;
			//joystickPos = Neutral;
		}
		ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
		adc_channel = 0;
	}
	
	ADC0.INTFLAGS |= ADC_RESRDY_bm;
}
*/

ISR(TCA0_OVF_vect){
	//PORTC.OUTTGL = 0x01;
	//Do something to the Tetris game, such as dropping the block...
	drop++;
	update = 1;
	//Not using the ADC any more. Sick and tired of the resets.
	//ADC0.COMMAND = ADC_START_IMMEDIATE_gc;
	TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
}