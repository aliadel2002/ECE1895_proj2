// Define LED pins
const int LED1 = 6;
const int LED2 = 7;
const int LED3 = 8;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  // LED 1
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED1, LOW);

  // LED 2
  digitalWrite(LED2, HIGH);
  delay(1000);
  digitalWrite(LED2, LOW);

  // LED 3
  digitalWrite(LED3, HIGH);
  delay(1000);
  digitalWrite(LED3, LOW);
}
// Tyler Stoops
