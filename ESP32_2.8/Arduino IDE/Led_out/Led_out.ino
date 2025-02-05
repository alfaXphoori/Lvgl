
#define LED_R 4
#define LED_G 16
#define LED_B 17

void setup() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
  }

void loop() {
    
  // Red color (full red, no green, no blue)
  analogWrite(LED_R, 255-255);
  analogWrite(LED_G, 255-0);
  analogWrite(LED_B, 255-0);

  delay(1000);

  // Green color (no red, full green, no blue)
  analogWrite(LED_R, 255-0);
  analogWrite(LED_G, 255-255);
  analogWrite(LED_B, 255-0);

  delay(1000);

  // Blue color (no red, no green, full blue)
  analogWrite(LED_R, 255-0);
  analogWrite(LED_G, 255-0);
  analogWrite(LED_B, 255-255);
    
  delay(1000);

  }
