#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN 6

LiquidCrystal_I2C lcd(0x27, 16, 2);

typedef enum {
  STATE_SECURED,
  STATE_NOT_SECURED
} SystemState;

void handleSecuredState();
void handleNotSecuredState();

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
    if (phoneDetected && isAtSpeed && currentState != STATE_SECURED) {
    currentState = STATE_SECURED;
    handleSecuredState();
  } else if ((!phoneDetected || !isAtSpeed) && currentState != STATE_NOT_SECURED) {
    currentState = STATE_NOT_SECURED;
    handleNotSecuredState();
  }
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


