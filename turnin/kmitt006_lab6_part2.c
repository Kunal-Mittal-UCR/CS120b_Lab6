/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 0;
unsigned long _avr_timer_cntcurr = 0;


int lights[] = {0x01, 0x02, 0x04, 0x02};
enum States{init, Light, stop} lightStates;
unsigned int i = 0;
unsigned char tmpA = 0x00;
unsigned char tmpB = 0x00;
unsigned char counter = 1;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr =_avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void tick(){
	switch(lightStates){
		case init:
			lightStates = Light;
			i = 0;
			break;
		case Light:
			tmpB = lights[i % 4];
			i+=counter;
			lightStates = Light;
			if(tmpA & 0x01)
				lightStates = stop;
			break;
		case stop:
			lightStates = stop;
			if(tmpA & 0x01)
				lightStates = Light;
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB  = 0xFF;
	PORTB = 0x00;
	TimerSet(300);
	TimerOn();
    /* Insert your solution below */
    while (1) {
		PORTA = PINA;
		tmpA = PORTA;
		tick();
		PORTB = tmpB;
		while(!TimerFlag);
		TimerFlag = 0;
    }
    return 1;
}
