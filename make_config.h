//#define DEVICE	atmega8 // atmega168
#define F_CPU	12000000
//#define FUSE_L  // # see below for fuse values for particular devices
//#define FUSE_H  // = 

// AVRDUDE = avrdude -c usbasp -p $(DEVICE) # edit this line for your programmer

// CFLAGS  = -Iusbdrv -I. -DDEBUG_LEVEL=0
// OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

// COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

