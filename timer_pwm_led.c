/*
 * timer_pwm_led.c
 *
 * Simple PWM to control brightness of LED using the Timer/Counter module and Capture channel on atxmega e
 * Tested on atxmega e xplained dev board, http://www.atmel.com/tools/xmega-e5xplained.aspx
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
#define TC0_CCAIF_bm TC4_CCAIF_bm
#define TC_CLKSEL_DIV1_gc TC45_CLKSEL_DIV1_gc
#define TC_CLKSEL_DIV64_gc TC45_CLKSEL_DIV64_gc
#define TC_CLKSEL_DIV1024_gc TC45_CLKSEL_DIV1024_gc
#define TC_WGMODE_FRQ_gc TC45_WGMODE_FRQ_gc
#define TC_SetCompareA( _tc, _compareValue ) ( (_tc)->CCABUF = (_compareValue) )

int main(void)
{
	LEDPORT.DIR |= LEDMASK;
	
	/* The CLKSEL bits in CTRLA is used to set the clock source for the counter 
	* see AVR1500 for details about gc, configuration masks.
	* 1MHz / 64 -> 1 tick every 64us. 156 (0x9c) such ticks for every 10ms (100Hz).
	* 100 Hz is fast enough to make flickering invisible for the human eye
	*
	* For start/stop of the timer see also:
	* CTRLGCLR/CTRLGSET – Control Register G Clear/Set, chapter 13.12.8 in the xmega e manual*/
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;  // Start Timer with prescaling 64

	/* In PWM this controls the period
	* The Period (PER) register (32bit) sets the TOP value for the counter, 
	* i.e. how far it will count */   
	TCC0.PER = 0x009c;  // 100 Hz at 1Mhz with prescaler at 64 
	
	/* In PWM this controls the duty time
	* Add code here that sets the timer compare value for channel A
	* ie. CCA register for the TCC0
	* The value that we set should be lower than the PER value.*/
	TC_SetCompareA( &TCC0,  0x0002);  // equivalent with TCC0.CCABUF = 0x0002

    while(1)
    {
		/* Control of period 
		* Check if the overflow flag (OVFIF) is set,
		* clear flag and toggle LEDs
		*/
		if((TCC0.INTFLAGS & TC0_OVFIF_bm) != 0)
		{
			TCC0.INTFLAGS = TC0_OVFIF_bm;      //Clear the IF by writing a logical 1 to the flag
			LEDPORT.OUTTGL = LEDMASK;          //Toggle LED
		}		
		
		// Control of duty time
		if((TCC0.INTFLAGS & TC0_CCAIF_bm) !=0)
		{
			TCC0.INTFLAGS = TC0_CCAIF_bm;      //Clear the TC_CCAIF by writing a logical 1 to the flag
			LEDPORT.OUTTGL = LEDMASK;          //Toggle LED
		}		
    }
}
