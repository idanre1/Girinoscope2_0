//-----------------------------------------------------------------------------
// Girino.h
//-----------------------------------------------------------------------------
// Copyright 2012 Cristiano Lino Fontana
//
// This file is part of Girino.
//
//	Girino is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	Girino is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with Girino.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <Arduino.h>

//-----------------------------------------------------------------------------
// Schmatics:
//-----------------------------------------------------------------------------
// D7:   Act as AIN1. Output thresholdPin(D3) via LP_filter will be connected to that input.
// D6:   Act as AIN0. Vin for sampled signal
// AREF: Connect with capacitor to 5V
// Vin:  ADCPIN aka Vin. Currently A0 connected to level shifter. A1 is connected to 1.8V
// LED:  When comparator trigger is latched, led is high
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Defines and Typedefs
//-----------------------------------------------------------------------------

#define DEBUG		0

#define ADCBUFFERSIZE	1280
//#define ADCBUFFERSIZE	512

#define ADCPIN		0
#define errorPin	13
#define thresholdPin	3 // 3 Means D3

#define BAUDRATE	115200	// Baud rate of UART in bps
//#define BAUDRATE	9600	// Baud rate of UART in bps
#define COMMANDDELAY	10	// ms to wait for the filling of Serial buffer
//#define COMMANDDELAY	500	// ms to wait for the filling of Serial buffer
#define COMBUFFERSIZE	4	// Size of buffer for incoming numbers

#if DEBUG == 1
	#define dprint(expression) Serial.print("# "); Serial.print( #expression ); Serial.print( ": " ); Serial.println( expression )
	#define dshow(expression) Serial.println( expression )
#else
	#define dprint(expression) 
	#define dshow(expression)
#endif

// Defines for setting and clearing register bits
// From avr/sfr_defs.h: (SpecialFunctionRegisters)
// LHS assign of reg: #define _SFR_BYTE(sfr) _MMIO_BYTE(_SFR_ADDR(sfr))
// Put 1 in the right place: #define _BV(bit) (1 << (bit))
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void initPins(void);
void initADC(void);
void initAnalogComparator(void);

void startADC( void );
void stopADC( void );
void startAnalogComparator( void );
void stopAnalogComparator( void );

void setADCPrescaler( uint8_t prescaler );
void setVoltageReference( uint8_t reference );
void setTriggerEvent( uint8_t event );

void error (void);
// Fills the given buffer with bufferSize chars from a Serial object
void fillBuffer( \
	char *buffer, \
	byte bufferSize, \
	HardwareSerial* serial = &Serial );
void printStatus(void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
extern volatile  boolean wait;
extern          uint16_t waitDuration;
extern volatile uint16_t stopIndex;
extern volatile uint16_t ADCCounter;
extern volatile  uint8_t ADCBuffer[ADCBUFFERSIZE];
extern volatile  boolean freeze;
extern volatile  boolean crln;

extern           uint8_t prescaler;
extern           uint8_t triggerEvent;
extern           uint8_t threshold;

extern              char commandBuffer[COMBUFFERSIZE+1];

