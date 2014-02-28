#include <msp430.h>
#include "ws2812/ws2812.h"

#define NUMBEROFLEDS	15
#define ENCODING 		3		// possible values 3 and 4


void configClock(void);
void configSPI(void);
void sendBuffer(uint8_t* buffer, ledcount_t ledCount);


int main(void) {

	// buffer to store encoded transport data
	uint8_t frameBuffer[(ENCODING * sizeof(ledcolor_t) * NUMBEROFLEDS)] = { 0, };

	// array with 15 rbg colors
	ledcolor_t leds[NUMBEROFLEDS] = {
			{ 0xFF, 0x00, 0x00 },
			{ 0x00, 0xFF, 0x00 },
			{ 0x00, 0x00, 0xFF },
			{ 0x7F, 0x00, 0x00 },
			{ 0x00, 0x7F, 0x00 },
			{ 0x00, 0x00, 0x7F },
			{ 0x3F, 0x00, 0x00 },
			{ 0x00, 0x3F, 0x00 },
			{ 0x00, 0x00, 0x3F },
			{ 0x1F, 0x00, 0x00 },
			{ 0x00, 0x1F, 0x00 },
			{ 0x00, 0x00, 0x1F },
			{ 0x0F, 0x00, 0x00 },
			{ 0x00, 0x0F, 0x00 },
			{ 0x00, 0x00, 0x0F },
	};

	uint8_t update;
	ledcolor_t blankLed = {0x00, 0x00, 0x00};
	uint8_t colorIdx;
	ledcolor_t led = {0,0,0};

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	configClock();
	configSPI();

	while(1) {

		// blank all LEDs
		fillFrameBufferSingleColor(&blankLed, NUMBEROFLEDS, frameBuffer, ENCODING);
		sendBuffer(frameBuffer, NUMBEROFLEDS);
		__delay_cycles(0xFFFFF);

		// set one LED after an other (one more with each round) with the colors from the LEDs array
		fillFrameBuffer(leds, NUMBEROFLEDS, frameBuffer, ENCODING);
		for(update=1; update <= NUMBEROFLEDS; update++) {
			sendBuffer(frameBuffer, update);
			__delay_cycles(0xFFFFF);
		}

		// set all LEDs with the same color and simulate a sunrise
		for(colorIdx=0; colorIdx < 0xFF; colorIdx++) {
			led.red = colorIdx + 1;
			fillFrameBufferSingleColor(&led, NUMBEROFLEDS, frameBuffer, ENCODING);
			sendBuffer(frameBuffer, NUMBEROFLEDS);
			__delay_cycles(0x1FFFF);
		}
		for(colorIdx=0; colorIdx < 0xD0; colorIdx++) {
			led.green = colorIdx;
			fillFrameBufferSingleColor(&led, NUMBEROFLEDS, frameBuffer, ENCODING);
			sendBuffer(frameBuffer, NUMBEROFLEDS);
			__delay_cycles(0x2FFFF);
		}
		for(colorIdx=0; colorIdx < 0x50; colorIdx++) {
			led.blue = colorIdx;
			fillFrameBufferSingleColor(&led, NUMBEROFLEDS, frameBuffer, ENCODING);
			sendBuffer(frameBuffer, NUMBEROFLEDS);
			__delay_cycles(0x3FFFF);
		}
		_nop();
	}
	return 0;
}


// copy bytes from the buffer to SPI transmit register
// should be reworked to use DMA
void sendBuffer(uint8_t* buffer, ledcount_t ledCount) {
	uint16_t bufferIdx;
	for (bufferIdx=0; bufferIdx < (ENCODING * sizeof(ledcolor_t) * ledCount); bufferIdx++) {
		while (!(UCA0IFG & UCTXIFG));		// wait for TX buffer to be ready
		UCA0TXBUF_L = buffer[bufferIdx];
	}
	__delay_cycles(300);
}

// configure MCLK and SMCLK to be sourced by DCO with a frequency of
//   8Mhz (3-bit encoding)
// 6.7MHz (4-bit encoding)
void configClock(void) {
	CSCTL0_H = 0xA5;
#if ENCODING == 3
	CSCTL1 = DCOFSEL_3;            // DCO frequency setting = 8MHz
#else
	CSCTL1 = DCOFSEL_2;            // DCO frequency setting = 6.7MHz
#endif
	CSCTL2 = SELS__DCOCLK + SELM__DCOCLK;
	CSCTL3 = DIVS__1 + DIVM__1;

}

void configSPI(void) {
	UCA0CTLW0 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTLW0 |= UCMST + UCSYNC + UCCKPL + UCMSB;	// 3-pin, 8-bit SPI master
													// Clock polarity high, MSB
	UCA0CTLW0 |= UCSSEL__SMCLK;                     // SMCLK

#if ENCODING == 3
	UCA0BR0 = 3;		// SPICLK 8MHz/3 = 2.66MHz
	UCA0BR1 = 0;
#else
	UCA0BR0 = 2;		// SPICLK 6.7MHz/2 = 3.35MHz
	UCA0BR1 = 0;
#endif
	UCA0MCTLW = 0;                            // No modulation
	UCA0CTLW0 &= ~UCSWRST;

	// UCB0SIMO	= LED data signal on P2.0
	P2SEL0 &= ~BIT0;
	P2SEL1 |= BIT0;
}
