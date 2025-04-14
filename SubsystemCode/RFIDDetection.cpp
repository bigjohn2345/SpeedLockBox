#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);
bool phoneDetected = false;

void setup() {
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  phoneDetected = false; // Reset before checking

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
}
//other test code

 if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Authorized UID
    byte authorizedUID[] = { 0x63, 0x63, 0x07, 0x04 };
    byte uidLength = sizeof(authorizedUID);

    isAuthorized = (rfid.uid.size == uidLength);
    for (int i = 0; i < uidLength && isAuthorized; i++) {
      if (rfid.uid.uidByte[i] != authorizedUID[i]) {
        isAuthorized = false;
      }
    }

    if (isAuthorized) {
      Serial.print(" Authorized Tag Detected: ");
    } else {
      Serial.print("Unauthorized Tag: ");
    }

    // Print UID
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
      if (i < rfid.uid.size - 1) Serial.print(":");
    }
    Serial.println();

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // Detect presence of card continuously (even if it's not new)
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Same UID check again (to maintain lock while present)
    byte authorizedUID[] = { 0x63, 0x63, 0x07, 0x04 };
    byte uidLength = sizeof(authorizedUID);

    bool cardStillPresent = (rfid.uid.size == uidLength);
    for (int i = 0; i < uidLength && cardStillPresent; i++) {
      if (rfid.uid.uidByte[i] != authorizedUID[i]) {
        cardStillPresent = false;
      }
    }

    doorLocked = cardStillPresent;
  } else {
    doorLocked = false;  // Card removed
  }
