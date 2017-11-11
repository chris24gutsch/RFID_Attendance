#pragma once //Keep GCC happy

#include <SoftwareSerial.h> // communication between ESP and RFID reader
#include "SparkFun_UHF_RFID_Reader.h" //Library for controlling the M6E Nano module

// Ensure the ESP shield 5V pin is NOT connected to the RFID shield.
//   Clip off the pin is an easy but "permanent" solution.
//   This should already have been done.

// Use a male-female jumper wire to connect the ESP's 5V pin to the RFID shield's
//   external power input (+) pin.

// jumper wires from:
//   GPIO 13 to shield 2
//   GPIO 15 to shield 3
// select "SW-UART" on RFID board
// move UART switch to "HW" on the ESP board


// For the buzzer, connect these pins between the ESP and RFID reader:
//   GPIO 12 to shield 9
//   GPIO 14 to shield 10
#define BUZZER1 12
#define BUZZER2 14

// Create instances of the serial driver and the RFID driver
SoftwareSerial softSerial(13, 15); //RX, TX (from ESP's view)
RFID nano;

//Initializes the RFID Scanner
//@param debug:
//	Summary: takes a boolean input to turn debugging on or off
void RFIDInit(bool debug) {
	// Setup the pins for the buzzer
	pinMode(BUZZER1, OUTPUT);
	pinMode(BUZZER2, OUTPUT);
	digitalWrite(BUZZER2, LOW); //Pull half the buzzer to ground and drive the other half.

	//Check if debugging
	if (debug == 1) {
		nano.enableDebugging(Serial);
	}

	// now get the RFID reader setup
	Serial.println();
	Serial.println("Initializing RFID reader...");

	// nano defaults to 115200 baud rate, keep it the same!
	softSerial.begin(115200);
	while (!softSerial);  // wait until open

	if (setupNano(nano, softSerial) == false)
	{
		Serial.println("Module failed to initialize. Check wiring?");
		status(ERROR);
		delay(1000);
		Serial.println("Restarting...");
		ESP.restart();
		while (1); //Freeze!
	}

	nano.setReadPower(500); //5.00 dBm. Higher values may cause USB port to brown out
							//Max Read TX Power is 27.00 dBm and may cause temperature-limit throttling
}
