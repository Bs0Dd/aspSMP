/* Name: main.c
 * Project: SMP reader device for Electronika MK-90
 * Creation Date: 2009-07-31
 * Copyright: (c) 2009 by Ilya Danilov
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

/* LEDS */
#define RDLED_PORT_DDR        DDRC
#define RDLED_PORT_OUTPUT     PORTC
#define RDLED_BIT             PC0
#define RDLED_ENABLE {RDLED_PORT_OUTPUT &= ~_BV(RDLED_BIT);}
#define RDLED_DISABLE {RDLED_PORT_OUTPUT |= _BV(RDLED_BIT);}

#define WRLED_PORT_DDR        DDRC
#define WRLED_PORT_OUTPUT     PORTC
#define WRLED_BIT             PC1
#define WRLED_ENABLE {WRLED_PORT_OUTPUT &= ~_BV(WRLED_BIT);}
#define WRLED_DISABLE {WRLED_PORT_OUTPUT |= _BV(WRLED_BIT);}

/* SMP interface */
#define SELPIN_PORT_DDR        DDRB
#define SELPIN_PORT_OUTPUT     PORTB
#define SELPIN_BIT             PB5
#define CLKPIN_PORT_DDR        DDRD
#define CLKPIN_PORT_OUTPUT     PORTD
#define CLKPIN_BIT             PD1
#define DATAPIN_PORT_DDR       DDRD
#define DATAPIN_PORT_OUTPUT    PORTD
#define DATAPIN_PORT_INPUT     PIND
#define DATAPIN_BIT            PD0

#define P_DELAY                10

#include "make_config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
#include "requests.h"       /* The custom request numbers we use */

/* ----------------------------- MK90 Specific ----------------------------- */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void sendByte(uint8_t b) {
	for (uint8_t mask=0x80; mask!=0; mask>>=1)
		{
			if (b & mask) {
				DATAPIN_PORT_OUTPUT |= _BV(DATAPIN_BIT);
			} else {
				DATAPIN_PORT_OUTPUT &= ~_BV(DATAPIN_BIT);
			}
			CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT);
			_delay_us(P_DELAY);
			CLKPIN_PORT_OUTPUT &= ~_BV(CLKPIN_BIT);
			_delay_us(P_DELAY);
		}
}

uint8_t readByte() {
	uint8_t x = 0;
	for (uint8_t mask=0x80; mask!=0; mask>>=1)
		{
			CLKPIN_PORT_OUTPUT &= ~_BV(CLKPIN_BIT);
			_delay_us(P_DELAY);
			CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT);
			_delay_us(P_DELAY);
			if (DATAPIN_PORT_INPUT & _BV(DATAPIN_BIT)) x |= mask;
		}
	return x;
}

void sendAddress(uint16_t addr) {
	DATAPIN_PORT_OUTPUT &= ~_BV(DATAPIN_BIT); // DATA=0
	_delay_us(P_DELAY);
	SELPIN_PORT_OUTPUT &= ~_BV(SELPIN_BIT);   // SELECT=0
	_delay_us(P_DELAY);
	sendByte (0xA0);            // WriteAddress cmd = 0xA0
	sendByte (addr/256); // заменить на low
	sendByte (addr%256); // заменить на high
	SELPIN_PORT_OUTPUT |= _BV(SELPIN_BIT);     // SELECT=1
	_delay_us(P_DELAY);
	DATAPIN_PORT_OUTPUT |= _BV(DATAPIN_BIT);   // DATA=1
	_delay_us(P_DELAY);
}

void startReading(uint16_t addr) {
	DATAPIN_PORT_DDR |= _BV(DATAPIN_BIT); // DATA - output
	sendAddress(addr);
	DATAPIN_PORT_OUTPUT &= ~_BV(CLKPIN_BIT); // DATA = 0
	_delay_us(P_DELAY);
	SELPIN_PORT_OUTPUT &= ~_BV(SELPIN_BIT);   // SELECT=0
	_delay_us(P_DELAY);
	sendByte (0xD0);            // ReadPostincrement cmd = 0xd0
	CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT);
	DATAPIN_PORT_DDR &= ~_BV(DATAPIN_BIT); // DATA - input
	_delay_us(P_DELAY);
}

void stopTalking() {
	SELPIN_PORT_OUTPUT |= _BV(SELPIN_BIT); // SELECT = 1 -> idle
	CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT); // CLK = 1 -> idle
	RDLED_DISABLE;
	WRLED_DISABLE;
}

void startWriting(uint16_t addr) {
	DATAPIN_PORT_DDR |= _BV(DATAPIN_BIT); // DATA - output
	sendAddress(addr);
	DATAPIN_PORT_OUTPUT &= ~_BV(CLKPIN_BIT); // DATA = 0
	_delay_us(P_DELAY);
	SELPIN_PORT_OUTPUT &= ~_BV(SELPIN_BIT);   // SELECT=0
	_delay_us(P_DELAY);
	sendByte (0xC0);            // WritePostincrement cmd = 0xc0
	CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT);
	_delay_us(P_DELAY);
}

void startIniting(uint16_t addr) {
	DATAPIN_PORT_DDR |= _BV(DATAPIN_BIT); // DATA - output
	sendAddress(addr);
	DATAPIN_PORT_OUTPUT &= ~_BV(CLKPIN_BIT); // DATA = 0
	_delay_us(P_DELAY);
	SELPIN_PORT_OUTPUT &= ~_BV(SELPIN_BIT);   // SELECT=0
	_delay_us(P_DELAY);
	sendByte (0x20);            // ErasePostdecrement cmd = 0x20
	CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT);
	_delay_us(P_DELAY);
}

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

static uint16_t	currentPosition, bytesRemaining;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;
	static uchar    dataBuffer[4];  /* buffer must stay valid when usbFunctionSetup returns */

	if (rq->bRequest == CUSTOM_RQ_READ_DATA){
		/* cartridge read requesed */
		currentPosition = 0;
		bytesRemaining = rq->wLength.word;
		startReading(rq->wValue.word);
		RDLED_ENABLE;
		return USB_NO_MSG;
	} else if (rq->bRequest == CUSTOM_RQ_WRITE_DATA){
		/* cartridge write requested */
		bytesRemaining = rq->wLength.word;
		startWriting(rq->wValue.word);
		WRLED_ENABLE;
		return USB_NO_MSG;
	} else if (rq->bRequest == CUSTOM_RQ_INIT_MEM){
		/* cartridge init requested */
		bytesRemaining = rq->wLength.word;
		startIniting(rq->wValue.word);
		WRLED_ENABLE;
		return USB_NO_MSG;
	}
	return 0;   /* default for not implemented requests: return no data back to host */
}

uchar usbFunctionRead(uchar *data, uchar len)
{
	uchar i;
	if(len > bytesRemaining) len = bytesRemaining; // len is max chunk size: send an incomplete chunk
	bytesRemaining -= len;
	for(i = 0; i < len; i++) {
		wdt_reset();
		data[i] = readByte();
	}
	if (!bytesRemaining) stopTalking();
	return len;                             // return real chunk size
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
	uchar i;
	if(len > bytesRemaining) len = bytesRemaining;
	bytesRemaining -= len;
	for(i = 0; i < len; i++) {
		wdt_reset();
		sendByte(data[i]);
	}
	if (!bytesRemaining) {
		stopTalking();
		return 1;
	} else {
		return 0;
	}
}

/* ------------------------------------------------------------------------- */

int main(void)
{
	uchar   i;

	wdt_enable(WDTO_1S);
	/* Even if you don't use the watchdog, turn it off here. On newer devices,
	 * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
	 */
	DBG1(0x00, 0, 0);       /* debug output: main starts */
	/* RESET status: all port bits are inputs without pull-up.
	 * That's the way we need D+ and D-. Therefore we don't need any
	 * additional hardware initialization.
	 */
	odDebugInit();
	usbInit();
	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	i = 0;
	while(--i){             /* fake USB disconnect for > 250 ms */
		wdt_reset();
		_delay_ms(1);
	}
	usbDeviceConnect();

	RDLED_PORT_DDR |= _BV(RDLED_BIT); /* Read  LED */
	WRLED_PORT_DDR |= _BV(WRLED_BIT); /* Write LED */

	SELPIN_PORT_DDR |= _BV(SELPIN_BIT);    // SELECT - output
	CLKPIN_PORT_DDR |= _BV(CLKPIN_BIT);    // CLK - output
	SELPIN_PORT_OUTPUT |= _BV(SELPIN_BIT); // SELECT = 1 -> idle
	CLKPIN_PORT_OUTPUT |= _BV(CLKPIN_BIT); // CLK = 1 -> idle
	// DATA - input

	sei();
	DBG1(0x01, 0, 0);       /* debug output: main loop starts */

	for(;;){                /* main event loop */
		DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
		wdt_reset();
		usbPoll();
	}
	return 0;
}

/* ------------------------------------------------------------------------- */
