//--Team 21rd 2025--//
//--GPS Subsystem Code: Sets bool "IsAtSpeed" true when above x mph speed--//
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
bool IsAtSpeed=false;
bool devicecharging=FALSE;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  pinMode(13, OUTPUT)
  
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop()
{
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }

  if(doorlatched) {    // check that door is latched
    digitalWrite(13, HIGH);
    devicecharging = TRUE;
  }

  else if(!doorlatched) {
    digitalWrite(13, LOW);
    devicecharging = FALSE;
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
    Serial.print(rmc.speed*2.23694);

    if (((rmc.speed)*2.23694)>15) {
      IsAtSpeed=true;
      Serial.print(" AtSpeed");
    } else {IsAtSpeed=false;}

  } else {
    Serial.print("No Fix");
    IsAtSpeed=false;
  }

  Serial.println();
}

void onGgaUpdate(nmea::GgaData const gga) {
}
