/*
 * io_interrupt_01.c
 *
 * Control an LED with a button through interrupts
 * Device ATxmega8e5 (AtmelStudio7 simulation was used as debugger/programmer tool)
 * Some refs:
 * - xmegae manual http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42005-8-and-16-bit-AVR-Microcontrollers-XMEGA-E_Manual.pdf
 * - AVR1305 http://www.atmel.com/Images/doc8043.pdf
 * - AVR1313 http://www.atmel.com/images/doc8050.pdf
 * - AVR1503 http://www.atmel.com/Images/doc8312.pdf
 * - http://www.avrfreaks.net/forum/external-interrupt-xmega-asf
 * - Some c-example mtrl: https://github.com/fffaraz/Introduction-to-Microprocessors/tree/master/material/AVR
 * - Other application notes for xmega series http://www.microchip.com/wwwproducts/en/ATxmega8E5
 *     
 * Created: 2017-06-09 16:28:09
 * Author : AJ
 */ 
 
#include <avr/io.h>
#include <avr/interrupt.h>

#define LEDPORT PORTA    // the port
#define LEDMASK PIN0_bm  // the pin mask
#define BTNPORT PORTA
#define BTNMASK PIN2_bm  //On xmegae5 pin 2 has full async sense
#define BTNINTVECT PORTA_INT_vect  
#define F_CPU 1000000UL  // 1MHz 
#include <util/delay.h>  // needed for _delay_ms()
 
// interrupt service routine 
ISR(BTNINTVECT) {
	LEDPORT.OUTTGL = LEDMASK;  //Toggle LED upon button interrupt
	LEDPORT.INTFLAGS = BTNMASK;  //Clear the interrupt flag. Seems that it must be done manually
}

// interrupt setup
void initInterrupt(void) {
	/* 5 steps (see AVR1313 and http://www.avrfreaks.net/forum/external-interrupt-xmega-asf):
	*  1. PINnCTRL. Input/sense config for each pin to support interrupt
	*  2. INT0MASK. Mask for the pins that will trigger. XMEGA E has only one intrrupt vect, ie INTMASK
	*  3. INTCTRL. Select the interrupt prio
	*  4. PMIC.CTRL. Must enable low level interrupts
	*  5. sei(); CPU.SREG. Global interrupt enable
	*
	*  In this setup pin 2 sense both edges
	*/
	BTNPORT.PIN2CTRL = PORT_ISC_BOTHEDGES_gc;  //1.Sense rising edge
	BTNPORT.INTMASK = BTNMASK;                 //2.Set pin trigger
	BTNPORT.INTCTRL = PORT_INTLVL_LO_gc;       //3.Set prio
	PMIC.CTRL = PMIC_LOLVLEN_bm;               //4.Enable  
	sei();                                     //5.
}
 
int main(void)
{	
	// Setup ports as in and out
	LEDPORT.DIR = LEDMASK;     // out
	BTNPORT.DIRCLR = BTNMASK;  // in
	LEDPORT.OUTCLR = LEDMASK;  // reset led
    
	initInterrupt();
	
    while(1) {
		_delay_ms(1000);  //let cpu sleep
	}

	return 0;
}
