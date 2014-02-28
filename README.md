Software functions for MSP430 to drive WS2812/B RGB LEDs via one-wire bus

The SPI peripheral will be used in cooperation with one of two transport stream encodings schemes.
One 3-bit and one 4-bit encoding was implemented for a SPI clock speed as low as 2.66MHz.
```
/*
 ******************
 * 3-bit encoding *
 ******************
 *
 * 8 bits from LED color stream encoded in 3 byte for transport stream (SPI TX)
 * or: 1 bit from LED color stream encoded in 3 bit for transport stream
 *
 *				_
 * ZERO: 100	 |__
 *	 	 	 	__
 * ONE : 110	  |_
 *
 * the bti in the middle defines the value
 *
 * data stream: 0x23		 		 0  0  1  0  0  0  1  1
 * encoding:						1x01x01x01x01x01x01x01x0
 * transport stream:				100100110100100100110110
 *
 * initial mask: 0x92 0x49 0x24		100100100100100100100100
 *
 * sourcebit :						 7  6  5  4  3  2  1  0
 * encoding  :						1x01x01x01x01x01x01x01x0
 * targetbit :						 6  3  0  5  2  7  4  1
 * targetbyte:						|   0   |   1   |   2   |
 *
 * sourcebit -> (targetbit,targetbyte)
 * 7->(6,0)
 * 6->(3,0)
 * 5->(0,0)
 * 4->(5,1)
 * 3->(2,1)
 * 2->(7,2)
 * 1->(4,2)
 * 0->(1,2)
 */


/*
 ******************
 * 4-bit encoding *
 ******************
 *
 * 2 bits from LED color stream encoded in 1 byte for transport stream (SPI TX)
 * or: 1 bit from LED color stream encoded in 4 bit for transport stream
 *
 * 						_
 * ZERO: 1000 = 0x8		 |___
 *	 	 	 	 	 	___
 * ONE : 1110 = 0xE		   |_
 *
 * SPI Clock around 3.2MHz (e.g. 6.7MHz/2 = 3.35MHz)
 *
 */
#define ZEROPATTERN		0x8	// 4-bit
#define ONEPATTERN		0xE	// 4-bit
															//			_     _
#define ZEROZEROPATTERN ((ZEROPATTERN<<4) | ZEROPATTERN)	// 10001000  |___| |___
															//			_     ___
#define ZEROONEPATTERN  ((ZEROPATTERN<<4) |  ONEPATTERN)	// 10001110  |___|   |_
															//			___   _
#define ONEZEROPATTERN  (( ONEPATTERN<<4) | ZEROPATTERN)	// 11101000    |_| |___
															//			___   ___
#define ONEONEPATTERN   (( ONEPATTERN<<4) |  ONEPATTERN)	// 11101110    |_|   |_
```
