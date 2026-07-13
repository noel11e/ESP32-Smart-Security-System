#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Keypad.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUZZER_PIN 4
#define LED_RED 2

#define PIR_PIN 34
#define REED_PIN 35

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool lastPirState = LOW;
bool lastDoorState = HIGH;

void showScreen(String line1, String line2 = "") {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(10, 10);
  display.println(line1);

  if (line2 != "") {
    display.setCursor(10, 35);
    display.println(line2);
  }

  display.display();
}

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String code = "";
String correctCode = "9890";

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(PIR_PIN, INPUT);
  pinMode(REED_PIN, INPUT);

  Wire.begin();

  if (!display.begin(0x3C, true)) {
    while (true);
  }

  showScreen("ENTER", "CODE");

  delay(8000);
}

void loop() {

  // ================= PIR (רציף + יציב) =================
  bool pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }

  if (pirState == HIGH && lastPirState == LOW) {
    showScreen("MOTION");
    tone(BUZZER_PIN, 1200, 150);
  }

  lastPirState = pirState;


  char key = keypad.getKey();

  if (key) {

    Serial.println(key);

    if (key == '*') {
      code = "";
      showScreen("RESET");
      delay(300);
      showScreen("ENTER", "CODE");
      return;
    }

    if (key == '#') {
      showScreen("ENTER", code);
    }

    if ((key >= '0' && key <= '9') ||
    key == 'A' || key == 'B' ||
    key == 'C' || key == 'D') {

  code += key;
  showScreen("CODE", code);
}

    if (code.length() == 4) {

      delay(200);

      if (code == correctCode) {

        showScreen("OK");

        tone(BUZZER_PIN, 523); delay(120);
        tone(BUZZER_PIN, 659); delay(120);
        tone(BUZZER_PIN, 784); delay(120);
        tone(BUZZER_PIN, 1047); delay(250);

        noTone(BUZZER_PIN);

      } else {

        showScreen("WRONG");

        for (int i = 0; i < 3; i++) {
          tone(BUZZER_PIN, 1000);
          delay(200);
          noTone(BUZZER_PIN);
          delay(100);
        }
      }

      delay(1500);

      code = "";
      showScreen("ENTER", "CODE");
    }
  }
}