// ----- Button pins -----
#define BTN_UP     2
#define BTN_DOWN   3
#define BTN_LEFT   4
#define BTN_RIGHT  5
#define BTN_FIRE   6

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);

  Serial.begin(115200);   // UART to ESP32
}

void loop() {
  if (digitalRead(BTN_UP) == LOW) {
    Serial.println("BTN_UP");
    delay(150);
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    Serial.println("BTN_DOWN");
    delay(150);
  }
  if (digitalRead(BTN_LEFT) == LOW) {
    Serial.println("BTN_LEFT");
    delay(150);
  }
  if (digitalRead(BTN_RIGHT) == LOW) {
    Serial.println("BTN_RIGHT");
    delay(150);
  }
  if (digitalRead(BTN_FIRE) == LOW) {
    Serial.println("BTN_FIRE");
    delay(150);
  }
}
