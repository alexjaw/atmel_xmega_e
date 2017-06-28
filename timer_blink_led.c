/*
 * timer_blink_led.c
 *
 * Blink LED using the Timer/Counter module on atxmega e
 * Based on examples in AVR1501, http://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en591816
 *
 * Created: 2017-06-28 11:24:31
 *  Author: alexander
 */ 
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define LEDPORT PORTD
#define LEDMASK PIN5_bm
// Using code from AVR1501 so need to redifne some macros for xmega_e mcu
#define TCC0 TCC4  //TCC4 is TC4_t struct
#define TC0_CLKSEL_gm TC4_CLKSEL_gm
#define TC0_OVFIF_bm TC4_OVFIF_bm
#define TC_CLKSEL_DIV1_gc TC45_CLKSEL_DIV1_gc
#define TC_CLKSEL_DIV1024_gc TC45_CLKSEL_DIV1024_gc

int main(void)
{
	LEDPORT.DIR |= LEDMASK;
	
	/* The CLKSEL bits in CTRLA is used to set the clock source for the counter 
	* see AVR1500 for details about gc, configuration masks.*/
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1024_gc;  // Start Timer with prescaling 1024

	/* The Period (PER) register (32bit) sets the TOP value for the counter, 
	* i.e. how far it will count
	* 1MHz clock with prescaler 1024 ticks every 1.024ms, which is 977 (976.56...) times per second.
	* So we let the TOP value of the counter be 0x03d0.*/    
	TCC0.PER = 0x03d0;	//  1Hz
	//TCC0.PER = 0x0062;  // 10Hz

    while(1)
    {
		/* Check if the overflow flag (OVFIF) is set,
		* clear flag and toggle LEDs
		*/
		if((TCC0.INTFLAGS & TC0_OVFIF_bm) != 0)
		{
			TCC0.INTFLAGS = TC0_OVFIF_bm;      //Clear the IF by writing a logical 1 to the flag
			LEDPORT.OUTTGL = LEDMASK;
		}
    }
}
