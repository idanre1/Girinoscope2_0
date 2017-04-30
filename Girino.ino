//-----------------------------------------------------------------------------
// Girino.ino
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

#include "Girino.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

volatile  boolean wait;
         uint16_t waitDuration;
volatile uint16_t stopIndex;
volatile uint16_t ADCCounter;
volatile  uint8_t ADCBuffer[ADCBUFFERSIZE];
volatile  boolean freeze;
volatile  boolean crln;

          uint8_t prescaler;
          uint8_t triggerEvent;
          uint8_t threshold;

             char commandBuffer[COMBUFFERSIZE+1];

//-----------------------------------------------------------------------------
// Main routines
//-----------------------------------------------------------------------------
//
// The setup function initializes registers.
//
void setup (void) {		// Setup of the microcontroller
	// Open serial port with a baud rate of BAUDRATE b/s
	Serial.begin(BAUDRATE);

	dshow("# setup()");
	// Clear buffers
	memset( (void *)ADCBuffer, 0, sizeof(ADCBuffer) );
	memset( (void *)commandBuffer, 0, sizeof(commandBuffer) );
	ADCCounter = 0;
	wait = false; // see IMPROVE . stopIndex replaces "wait" behavior
	waitDuration = ADCBUFFERSIZE - 32;
	//IMPROVE: stopIndex = -1;
	stopIndex = ADCBUFFERSIZE + 1; //IMPROVE
	freeze = false;

	prescaler = 128;
	triggerEvent = 3;

	threshold = 127;

	crln = false;

	// Activate interrupts
	sei();

	initPins();
	initADC();
	initAnalogComparator();

	Serial.println("Girino ready");
	//printStatus();
}

void loop (void) {
//	dprint(ADCCounter);
//	dprint(stopIndex);
//	dprint(wait);
//	dprint(freeze);
	#if DEBUG == 1
//	Serial.println( ADCSRA, BIN );
//	Serial.println( ADCSRB, BIN );
	#endif

	// If freeze flag is set, then it is time to send the buffer to the serial port
	if ( freeze )
	{
		dshow("# Frozen");

		// Send buffer through serial port in the right order
		//Serial.print("Buffer: ");
		//Serial.write( ADCBuffer, ADCBUFFERSIZE );
		//Serial.print("End of Buffer");
		Serial.write( (uint8_t *)ADCBuffer + ADCCounter, ADCBUFFERSIZE - ADCCounter );
		Serial.write( (uint8_t *)ADCBuffer, ADCCounter );

		if (crln == true)
			Serial.println("End of Buffer");

		// Turn off errorPin
		//digitalWrite( errorPin, LOW );
		cbi(PORTB,PORTB5);

		wait = false; // see IMPROVE . stopIndex replaces "wait" behavior
		stopIndex = ADCBUFFERSIZE + 1; //IMPROVE
		freeze = false;

		// Clear buffer
		//memset( (void *)ADCBuffer, 0, sizeof(ADCBuffer) );

		//startADC();
		// Let the ADC fill the buffer a little bit
		//delay(1);
		//startAnalogComparator();

		#if DEBUG == 1
		delay(3000);
		#endif
	}

	if ( Serial.available() > 0 ) {
		// Read the incoming byte
		char theChar = Serial.read();
			// Parse character
		switch (theChar) {
			case 's':			// 's' for starting ADC conversions
//				delay(COMMANDDELAY);
//				Serial.println("ADC conversions started");

				// Clear buffer
				memset( (void *)ADCBuffer, 0, sizeof(ADCBuffer) );

				startADC();
				// Let the ADC fill the buffer a little bit
				//delay(1);
				startAnalogComparator();
				break;
			case 'S':			// 'S' for stopping ADC conversions
				//Serial.println("ADC conversions stopped");
				stopAnalogComparator();
				stopADC();
				break;
			case 'c':			// 'c' for <CR> toggeling
			case 'C':			// 'c' for <CR> toggeling
				if (crln == true)
					crln = false;
				else
					crln = true;
					Serial.print("Setting CRln after sending data to: ");
					Serial.println(crln);
				break;
			case 'p':			// 'p' for new prescaler setting
			case 'P': {
				// Wait for COMMANDDELAY ms to be sure that the Serial buffer is filled
				delay(COMMANDDELAY);

				fillBuffer( commandBuffer, COMBUFFERSIZE );

				// Convert buffer to integer
				uint8_t newP = atoi( commandBuffer );

				// Display moving status indicator
				Serial.print("Setting prescaler to: ");
				Serial.println(newP);

				prescaler = newP;
				setADCPrescaler(newP);
				}
				break;

			case 'r':			// 'r' for new voltage reference setting
			case 'R': {
				// Wait for COMMANDDELAY ms to be sure that the Serial buffer is filled
				delay(COMMANDDELAY);

				fillBuffer( commandBuffer, COMBUFFERSIZE );

				// Convert buffer to integer
				uint8_t newR = atoi( commandBuffer );

				// Display moving status indicator
				Serial.print("Setting voltage reference to: ");
				Serial.println(newR);

				setVoltageReference(newR);
				}
				break;

			case 'e':			// 'e' for new trigger event setting
			case 'E': {
				// Wait for COMMANDDELAY ms to be sure that the Serial buffer is filled
				delay(COMMANDDELAY);

				fillBuffer( commandBuffer, COMBUFFERSIZE );

				// Convert buffer to integer
				uint8_t newE = atoi( commandBuffer );

				// Display moving status indicator
				Serial.print("Setting trigger event to: ");
				Serial.println(newE);

				triggerEvent = newE;
				setTriggerEvent(newE);
				}
				break;

			case 'w':			// 'w' for new wait setting
			case 'W': {
				// Wait for COMMANDDELAY ms to be sure that the Serial buffer is filled
				delay(COMMANDDELAY*2);

				fillBuffer( commandBuffer, COMBUFFERSIZE );

				// Convert buffer to integer
				uint16_t newW = atoi( commandBuffer );

				// Display moving status indicator
				Serial.print("Setting waitDuration to: ");
				Serial.println(newW);

				waitDuration = newW;
				}
				break;

			case 't':			// 'w' for new threshold setting
			case 'T': {
				// Wait for COMMANDDELAY ms to be sure that the Serial buffer is filled
				delay(COMMANDDELAY);

				fillBuffer( commandBuffer, COMBUFFERSIZE );

				// Convert buffer to integer
				uint8_t newT = atoi( commandBuffer );

				// Display moving status indicator
				Serial.print("Setting threshold to:");
				Serial.println(newT);

				threshold = newT;
				analogWrite( thresholdPin, threshold );
				}
				break;

			case 'd':			// 'd' for display status
			case 'D':
				printStatus();
				break;

//			case 'h':			// 'h' for help
//			case 'H':
//				Serial.println("HELP screen");
//				Serial.println("command: description. [Values]");
//				Serial.println("s      : start ADC conversion");
//				Serial.println("p      : new prescaler setting - division factor between the system clock [2,4,8,16,32,64,128]");
//				Serial.println("r      : new voltage reference setting [0 off, 1-default AREF, 2 internal 1.1v]");
//				Serial.println("e      : new trigger event setting [0- toggle, 2 falling, 3-default rising]");
//				Serial.println("w      : new wait duration. how many cycles to sample after trigger [0:ADCBUFFERSIZE]. default (ADCBUFFERSIZE - 32)");
//				Serial.println("t      : new threshold voltage setting. [0:255] fraction of 5V. default 127 meaning 2.5v");
//			
//				break;

			default:
				// Display error message
				Serial.print("ERROR: Command not found, it was: ");
				Serial.println(theChar);
				error();
		}
	}
}
