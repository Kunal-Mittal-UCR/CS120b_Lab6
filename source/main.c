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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;



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

unsigned char tmpA = 0x00;
unsigned char tmpB = 0x00;
unsigned char periodCounter = 0;

enum States{start, wait, increment, decrement, reset} counterState;

void tick(){
  switch(counterState){
    case start:
      tmpB = 0x07;
      counterState = wait;
	  periodCounter = 0;
      break;
	  
    case wait:
		periodCounter = 0;
		tmpB = 0xFF;
      if((tmpA & 0x01) && !(tmpA & 0x02)){
        counterState = increment; 
        if(tmpB < 9)
          tmpB++;
      }
      else if(!(tmpA & 0x01) && (tmpA & 0x02)){
        counterState = decrement; 
        if(tmpB > 0)
          tmpB--;
      }
      else if((tmpA & 0x01) && (tmpA & 0x02)){
        counterState = reset; 
        tmpB = 0;
      }
      else if(!(tmpA & 0x01) && !(tmpA & 0x02))
        counterState = wait;

      break;
	  
    case increment:
      if((tmpA & 0x01) && !(tmpA & 0x02)){
        counterState = increment;
		if(periodCounter <= 10)
			periodCounter++;
		else if(periodCounter > 10){
			if(tmpB < 9){
				tmpB++;
				periodCounter = 0;
			}
		}
      }
      else if(!(tmpA & 0x01) && (tmpA & 0x02)){
        counterState = decrement;
		periodCounter = 0;
        if(tmpB > 0)
          tmpB--;
      }
      else if((tmpA & 0x01) && (tmpA & 0x02)){
        counterState = reset; 
        tmpB = 0;
      }
      else if(!(tmpA & 0x01) && !(tmpA & 0x02))
        counterState = wait;
      break;

    case decrement:
      if((tmpA & 0x01) && !(tmpA & 0x02)){
		  periodCounter = 0;
        counterState = increment;
        if(tmpB < 9)
          tmpB++;
      }
      else if(!(tmpA & 0x01) && (tmpA & 0x02)){
        counterState = decrement;
		if(periodCounter <= 10)
			periodCounter++;
		if(periodCounter > 10){
			if(tmpB > 0){
				tmpB--;
				periodCounter = 0;
			}
		}
      }
      else if((tmpA & 0x01) && (tmpA & 0x02)){
        counterState = reset; 
        tmpB = 0;
      }
      else if(!(tmpA & 0x01) && !(tmpA & 0x02))
        counterState = wait;
      break;

    case reset:
		periodCounter = 0;
      if((tmpA & 0x01) && (tmpA & 0x02)){
        counterState = reset; 
        tmpB = 0;
      }
      else if(!(tmpA & 0x01) && !(tmpA & 0x02))
        counterState = wait;       
      else if((tmpA & 0x01) && !(tmpA & 0x02)){
        counterState = increment;
        if(tmpB < 9)
          tmpB++;
      }
      else if(!(tmpA & 0x01) && (tmpA & 0x02)){
        counterState = increment;
        if(tmpB > 0)
          tmpB--;
      }
      break;
  } 
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; tmpA = 0x00; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; tmpB = 0x00;
    /* Insert your solution below */
	TimerSet(100);
	TimerOn();
	
	counterState = start;
    while (1) {
		PORTA = ~PINA;
		tmpA = PORTA & 0x03;
		tick();
		PORTB = tmpB;
		while(!TimerFlag);
		TimerFlag = 0;
    }
    return 1;
}
