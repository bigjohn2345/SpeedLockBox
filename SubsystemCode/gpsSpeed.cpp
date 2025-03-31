/*
 * This example demonstrates how to use the 107-Arduino-NMEA library
 * to parse the incoming NMEA messages and make use of the extracted
 * information.
 *
 * Hardware:
 *   - Arduino MKR Family Board, e.g. MKR VIDOR 4000
 *   - Adafruit Mini GPS PA1010D Module
 *
 * Electrical Connection:
 *   - GPS Module VIN <->      VCC MKR VIDOR 4000
 *   - GPS Module GND <->      GND MKR VIDOR 4000
 *   - GPS Module TXO <-> (13) RX  MKR VIDOR 4000
 *   - GPS Module RXI <-> (14) TX  MKR VIDOR 4000
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <ArduinoNmeaParser.h>

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

ArduinoNmeaParser parser(onRmcUpdate, onGgaUpdate);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop()
{
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }
}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

void onRmcUpdate(nmea::RmcData const rmc)
{
  if (rmc.is_valid)
  {
    Serial.print(" Speed: ");
    Serial.print(rmc.speed);
  }

  Serial.println();
}

void onGgaUpdate(nmea::GgaData const gga) {
}
