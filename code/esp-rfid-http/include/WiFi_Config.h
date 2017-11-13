#pragma once //Keep GCC happy

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "util.h"

//Prints the ESP8266's MAC address
uint8_t printMAC() {
	byte mac[6];

	WiFi.macAddress(mac);
	Serial.println("");
	Serial.print("MAC: ");
	for (uint8_t i = 0; i < sizeof(mac); i++) {
		Serial.print(mac[i], HEX);
		if (i == (sizeof(mac)-1)) {
			Serial.println("");
		}
		else {
			Serial.print(":");
		}
	}
	
	return WL_SCAN_COMPLETED;
}

//Scans for networks
void networkScan() {
	Serial.println("Scan start ... ");

	int n = WiFi.scanNetworks();
	Serial.print(n);
	Serial.println(" network(s) found");
	for (int i = 0; i < n; i++) {
		Serial.println(WiFi.SSID(i));
	}
}

//Connects to a network given an SSID and password (if SSID is encrypted).
//@param ssid:
//	Summary: SSID of the network you would like to connect to.
//@param pass:
//	Summary: Password of the network you would like to connect to.
//			 Defaults to empty.
void setNetwork(char* ssid, char *pass = "") {
	if (pass == "") { //Unencrypted
		WiFi.begin(ssid);

		Serial.println("Checking Connection...");
		while (WiFi.status() != WL_CONNECTED) {
			status(WAITING);
		}
		Serial.println("Connected!");

		status(GOOD);
	}
	else { //Encrypted
		WiFi.begin(ssid, pass);

		Serial.println("Checking Connection...");
		while (WiFi.status() != WL_CONNECTED) {
			status(WAITING);
		}
		Serial.println("Connected!");

		status(GOOD);
	}

}

//Initializes the WiFi
//@param ssid:
//	Summary: SSID of the network you would like to connect to.
//@param pass:
//	Summary: Password of the network you would like to connect to.
//			 Defaults to empty.
uint8_t wifiInit(char* ssid, char *pass = "") {
	//Following are handled in ESP8266WiFiSTA.cpp but this gives more of an idea of what went wrong
	if (!ssid || *ssid == 0x00 || strlen(ssid) > 31) {
		Serial.println("SSID too long or missing!");
		return WL_CONNECT_FAILED;
	}

	if (pass && strlen(pass) > 64) {
		Serial.println("Password too long!");
		return WL_CONNECT_FAILED;
	}
	////////////////////////////////////////////////////

	//Setup client-only mode
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();

	printMAC();
	networkScan();
	setNetwork(ssid, pass);

	Serial.println("");
	WiFi.printDiag(Serial);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}
