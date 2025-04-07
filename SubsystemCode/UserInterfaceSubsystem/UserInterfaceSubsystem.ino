#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Create LCD using plain pointer to mimic C-style
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
#define BUTTON_TRUE_PIN 2
#define BUTTON_FALSE_PIN 3
#define BUZZER_PIN 9

// System states
typedef enum {
  STATE_SECURED,
  STATE_NOT_SECURED
} SystemState;

SystemState currentState = STATE_NOT_SECURED;

// Function declarations
void handleSecuredState();
void handleNotSecuredState();

void setup() {
  // Init LCD
  lcd.init();
  lcd.backlight();

  // Setup pins
  pinMode(BUTTON_TRUE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_FALSE_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Startup message
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Button state reads (active LOW)
  int truePressed = digitalRead(BUTTON_TRUE_PIN) == LOW;
  int falsePressed = digitalRead(BUTTON_FALSE_PIN) == LOW;

  if (truePressed && currentState != STATE_SECURED) {
    currentState = STATE_SECURED;
    handleSecuredState();
  } else if (falsePressed && currentState != STATE_NOT_SECURED) {
    currentState = STATE_NOT_SECURED;
    handleNotSecuredState();
  }

  // Buzzer behavior
  if (currentState == STATE_NOT_SECURED) {
    digitalWrite(BUZZER_PIN, HIGH);  // Loud continuous
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(100); // debounce
}

void handleSecuredState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Phone Secured");

  // Simple pleasant tone sequence
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
  static int direction = 1; // 1 = up, -1 = down

  unsigned long now = millis();
  if (now - lastUpdate > 20) { // update frequency every 20ms
    lastUpdate = now;

    tone(BUZZER_PIN, freq);

    freq += direction * 50; // step size

    if (freq >= 3000) direction = -1; // go back down
    if (freq <= 1000) direction = 1;  // go back up
  }
}




  // Continuous tone handled in loop


