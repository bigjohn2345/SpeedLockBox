
//--Team 21rd 2025--//
//--GPS Subsystem Code: Sets bool "IsAtSpeed" true when above x mph speed--//
/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <ArduinoNmeaParser.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Definitions

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 4

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

LiquidCrystal_I2C lcd(0x27, 16, 2);

typedef enum {
  STATE_SECURED,
  STATE_NOT_SECURED
} SystemState;

void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);
void handleSecuredState();
void handleNotSecuredState();

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

ArduinoNmeaParser parser(onRmcUpdate, onGgaUpdate);

MFRC522 rfid(SS_PIN, RST_PIN);

bool IsAtSpeed=false;

bool devicecharging=false;

Servo myservo;

bool doorLocked = false;

bool doorLockedprevState = false;

bool phoneDetected = false;

bool phoneDetectedprevState = false;

unsigned long TestDelay=0;

SystemState currentState = STATE_NOT_SECURED;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  myservo.attach(7);
  
  pinMode(6, OUTPUT);
  
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  rfid.PCD_Init(); 
  Serial.println("RFID Subsystem Initialized");

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();

}

void loop()
{
  //phoneDetected = false; // Reset before checking
  
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }

  if(IsAtSpeed) {
    if (rfid.PICC_IsNewCardPresent()) {
      if (rfid.PICC_ReadCardSerial()) {
        Serial.print("Phone Detected: ");

        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }

        Serial.println();
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();

        phoneDetected = true;
      }
    }
  }
  else {
    if(phoneDetected) {
      phoneDetected = false;
    }
  }

  if (phoneDetected && IsAtSpeed) {
    myservo.write(0);
    doorLocked = true;
    digitalWrite(6, HIGH);
    devicecharging = true;
  }
  else if(!phoneDetected || !IsAtSpeed) {
    myservo.write(90);
    doorLocked = false;
    digitalWrite(6, LOW);
    devicecharging = false;
  }
  
  //--TEST CODE FOR IsAtSpeed
  if ((millis()-TestDelay) >= 10000) {
    TestDelay=millis();
    IsAtSpeed=!IsAtSpeed;
    if(IsAtSpeed) {
      Serial.println("GPS test code at speed");
    }
    else {
      Serial.println("GPS test code not at speed");
    }
  }

  if (phoneDetected && IsAtSpeed && (currentState != STATE_SECURED)) {
    currentState = STATE_SECURED;
    handleSecuredState();
  } 

  else if ((!phoneDetected || !IsAtSpeed) && (currentState != STATE_NOT_SECURED)) {
    currentState = STATE_NOT_SECURED;
    handleNotSecuredState();
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

void handleSecuredState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Phone Secured");

  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  tone(BUZZER_PIN, 1500, 200);
  delay(300);
  tone(BUZZER_PIN, 2000, 200);
  delay(300);
  noTone(BUZZER_PIN);
}

void handleNotSecuredState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Phone Not");
  lcd.setCursor(0, 1);
  lcd.print("Secured");

  static unsigned long lastUpdate = 0;
  static int freq = 1000;
  static int direction = 1;

  unsigned long now = millis();
  if (now - lastUpdate > 20) {
    lastUpdate = now;
    tone(BUZZER_PIN, freq);
    freq += direction * 50;
    if (freq >= 3000) direction = -1;
    if (freq <= 1000) direction = 1;
  }
}
