//--Team 21rd 2025--//
//--GPS Subsystem Code: Sets bool "IsAtSpeed" true when above x mph speed--//
#include <ArduinoNmeaParser.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Pin Definitions
#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 7
#define STATUS_LED 8

// Function Declarations
void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);

// Global Variables
ArduinoNmeaParser parser(onRmcUpdate, onGgaUpdate);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo;

bool IsAtSpeed = false;
bool devicecharging = false;

bool doorLocked = false;
bool doorLockedprevState = false;

bool phoneDetected = false;

unsigned long TestDelay = 0;

void setup() {
  myservo.attach(SERVO_PIN);
  pinMode(STATUS_LED, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  Serial.println("RFID Subsystem Initialized");
}

void loop() {
  // Simulated speed change every 5 seconds (for testing without GPS)
  if ((millis() - TestDelay) >= 5000) {
    TestDelay = millis();
    IsAtSpeed = !IsAtSpeed;
    Serial.print("Test Speed Toggle -> IsAtSpeed: ");
    Serial.println(IsAtSpeed ? "TRUE" : "FALSE");
  }

  // Parse GPS data
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }

  // Check for RFID tag
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    phoneDetected = true;

    Serial.print("Phone Detected: ");
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
      if (i < rfid.uid.size - 1) Serial.print(":");
    }
    Serial.println();

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  } else {
    phoneDetected = false;
  }

  // Lock/unlock logic
  if (phoneDetected && IsAtSpeed) {
    doorLocked = true;
  } else {
    doorLocked = false;
  }

  // Update only if doorLocked state changed
  if (doorLocked != doorLockedprevState) {
    if (doorLocked) {
      myservo.write(0);               // Lock
      digitalWrite(STATUS_LED, HIGH); // LED on
      devicecharging = true;
    } else {
      myservo.write(90);              // Unlock
      digitalWrite(STATUS_LED, LOW);  // LED off
      devicecharging = false;
    }

    doorLockedprevState = doorLocked;
  }

  delay(100); // small delay for stability
}

// GPS Speed Parsing
void onRmcUpdate(nmea::RmcData const rmc) {
  if (rmc.is_valid) {
    float speed_mph = rmc.speed * 2.23694;
    Serial.print(" Speed: ");
    Serial.print(speed_mph);

    if (speed_mph > 15) {
      // IsAtSpeed = true;
      Serial.print(" AtSpeed");
    } else {
      // IsAtSpeed = false;
    }
  } else {
    Serial.print(" No GPS Fix");
    // IsAtSpeed = false;
  }

  Serial.println();
}

void onGgaUpdate(nmea::GgaData const gga) {
  // Not used, but available for satellite fix info
}
