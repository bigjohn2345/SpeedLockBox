
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
#define RST_PIN 9

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

bool devicecharging=false;

Servo myservo;

bool doorLocked = false;

bool phoneDetected = false;

unsigned long TestDelay=0;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  myservo.attach(7);
  
  pinMode(8, OUTPUT);
  
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  rfid.PCD_Init(); 
  Serial.println("RFID Subsystem Initialized");
}

void loop()
{
  //phoneDetected = false; // Reset before checking
  
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
      //rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }
  else {

    delay(250);
    phoneDetected = false;

  }
  
  if (phoneDetected && IsAtSpeed) {
    myservo.write(0);
    doorLocked = true;
  } else {
    myservo.write(90);
    doorLocked = false;
  }

  if(doorLocked) {    // check that door is latched
    digitalWrite(8, HIGH);
    devicecharging = true;
  }

  else if(!doorLocked) {
    digitalWrite(8, LOW);
    devicecharging = false;
  }
  
  //--TEST CODE FOR IsAtSpeed
  if ((millis()-TestDelay) >= 5000) {
    TestDelay=millis();
    Serial.println("GPS test code inc");
    IsAtSpeed=!IsAtSpeed;
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
      //IsAtSpeed=true;
      Serial.print(" AtSpeed");
    } else {
      //IsAtSpeed=false;
    }

  } else {
    Serial.print("No Fix");
    //IsAtSpeed=false;
  }

  Serial.println();
}

void onGgaUpdate(nmea::GgaData const gga) {
}
