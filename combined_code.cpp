//--Team 21rd 2025--//
//--GPS Subsystem Code: Sets bool "IsAtSpeed" true when above x mph speed--//
/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <ArduinoNmeaParser.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

//Definitions

#define SS_PIN 10
#define RST_PIN 5

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

ArduinoNmeaParser parser(onRmcUpdate, onGgaUpdate);

MFRC522 rfid(SS_PIN, RST_PIN);

bool IsAtSpeed=false;

bool devicecharging=FALSE;

Servo myservo;

bool doorLocked = false;

bool phoneDetected = false;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  myservo.attach(9);
  
  pinMode(13, OUTPUT)
  
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin()
  rfid.PCD_Init(); 
  Serial.println("RFID Subsystem Initialized");
}

void loop()
{
  phoneDetected = false; // Reset before checking
  
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }

  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      phoneDetected = true;
      Serial.print("Phone Detected: ");

      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }

      Serial.println();
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }
  
  if (phoneDetected && IsAtSpeed) {
    myservo.write(100);
    doorLocked = true;
  } else {
    myservo.write(180);
    doorLocked = false;
  }

  if(doorLocked) {    // check that door is latched
    digitalWrite(13, HIGH);
    devicecharging = TRUE;
  }

  else if(!doorLocked) {
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
