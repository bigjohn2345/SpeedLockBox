
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
  STATE_NOT_SECURED,
  STATE_IDLE
} SystemState;

void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);
void handleSecuredState();
void handleNotSecuredState();
void handleIdleState();

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

bool noise=true;

unsigned long lastUpdate=0;

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
  
  int speed = map(analogRead(A0), 0, 1023, 0 , 80);


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
        //rfid.PICC_HaltA();
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
  else if(!IsAtSpeed) {
    myservo.write(90);
    doorLocked = false;
    digitalWrite(6, LOW);
    devicecharging = false;
  }
  
  //--TEST CODE FOR IsAtSpeed
  //if ((millis()-TestDelay) >= 10000) {
  //  TestDelay=millis();
  //  IsAtSpeed=!IsAtSpeed;
  //  if(IsAtSpeed) {
  //    Serial.println("GPS test code at speed");
  //  }
  //  else {
  //    Serial.println("GPS test code not at speed");
  //  }
  //}
  if(speed >= 5) {
    IsAtSpeed = true;
    Serial.println(speed);
  }
  else {
    IsAtSpeed = false;
    Serial.println(speed);
  }

  if(!IsAtSpeed && (currentState != STATE_IDLE)) {
    currentState = STATE_IDLE;
    handleIdleState();
  }

  if (phoneDetected && (currentState != STATE_SECURED)) {
    currentState = STATE_SECURED;
    handleSecuredState();
  } 

  else if ((!phoneDetected && IsAtSpeed) && (currentState != STATE_NOT_SECURED)) {
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
/*
  unsigned long now = millis();
  static bool buzzerOn = false;

  // Toggle buzzer every 500 ms
  if (now - lastUpdate >= 500) {
    lastUpdate = now;
    if (buzzerOn) {
      noTone(BUZZER_PIN);  // Turn off buzzer
      buzzerOn = false;
    } else {
      tone(BUZZER_PIN, 3000);  // Turn on buzzer at 3000 Hz
      buzzerOn = true;
    }
  }

*/  static int freq = 3000;
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

void handleIdleState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Idle");
  noTone(BUZZER_PIN);
}

