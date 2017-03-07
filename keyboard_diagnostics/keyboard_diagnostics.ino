/*
   3/6/2017
   Custom Keyboard Arduino code

   Diagnostic code for shift registers. Useful for finding bad
   connections with clock and latch pins.

   by Jim Schmitz
   jim@ixora.io
   ITP Camp 2016
*/

int clockPin = 8; // yellow
int latchPin = 9; // green
int dataPin = 10; // orange

void setup() {
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(clockPin, HIGH);
  digitalWrite(latchPin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
}

