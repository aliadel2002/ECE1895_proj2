const int dataPin = 11;  // SER(14)
const int latchPin = 12; // RCLK(12)
const int clockPin = 13; // SRCLK(11)
const int button = 8;
int num = 0;

void setup() {
  pinMode(dataPin,OUTPUT);
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(button,INPUT_PULLUP);
}

// Lookup table for Common Cathode (Active HIGH)
const byte digitMap[] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67
};

void displayNumbers(int n1, int n2, int n3, int n4) {
  int nums[4] = {n1, n2, n3, n4}; // data into array
  
  digitalWrite(latchPin, LOW); // hold latch open

  // use shiftOut function for each number
  for (int i = 3; i >= 0; i--) {
    shiftOut(dataPin, clockPin, MSBFIRST, digitMap[nums[i]]);
  }

  digitalWrite(latchPin, HIGH); // close latch
}

void loop() {
  if(digitalRead(button)==LOW) {
    displayNumbers(num,num,num,num);
    num++;
    delay(200);
    if(num >= 10){
      num=0;
    }
  }
}
