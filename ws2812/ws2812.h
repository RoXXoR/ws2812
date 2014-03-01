#include <stdint.h>

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} ledcolor_t;

typedef uint16_t ledcount_t;

void fillFrameBuffer(ledcolor_t* leds, ledcount_t ledCount, uint8_t* buffer, uint8_t encoding);
void fillFrameBufferSingleColor(ledcolor_t* led, ledcount_t ledCount, uint8_t* buffer, uint8_t encoding);
void encodeData3bit(ledcolor_t* led, uint8_t* output);
void encodeData4bit(ledcolor_t* led, uint8_t* output);
