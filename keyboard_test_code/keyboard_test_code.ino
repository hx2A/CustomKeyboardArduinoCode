/*
   3/6/2017
   Custom Keyboard Arduino code

   Test code that identifies which shift register and input is being
   pressed.

   by Jim Schmitz
   jim@ixora.io
   ITP Camp 2016
*/

#include <Keyboard.h>

/*
   Arduino pin numbers for shift register control pins.
*/
int clockPin = 8; // yellow
int latchPin = 9; // green
int dataPin = 10; // orange

unsigned long last_key_press = 0;

// number of shift registers
int num_registers = 10;
// shift register inputs filled with shiftIn
byte register_states[] = {0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00
                         };

void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if (millis() - last_key_press > 450) {
    read_keyboard();
  }
  delay(25);
}

/*
   Interpret keyboard button presses via shift registers and send
   button identifier messages to Keyboard API.
*/
void read_keyboard() {
  digitalWrite(latchPin, LOW);

  // read shift registers and store in register_states
  for (int i = 0; i < num_registers; ++i) {
    register_states[i] = shiftIn(dataPin, clockPin);
  }

  digitalWrite(latchPin, HIGH);

  // read register states and print which buttons are being pressed
  int register_count = check_register_states();

  if (register_count > 0) {
    last_key_press = millis();
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/*
   Count the number of buttons being pressed simultaneously.

   If there is a short in the shift registers, it will behave as if
   every button linked to a single shift regsister (not necessarily
   the same one!!!) is pressed.

   Note that the Keyboard.h library can only handle 6 keys at the same
   time.
*/
int check_register_states() {
  int press_count = 0;

  for (int i = 0; i < num_registers; i++) {
    for (int j = 0; j < 8; j++) {
      if ( (1 << j) & register_states[i] ) {
        Keyboard.print("(register=");
        Keyboard.print(i);
        Keyboard.print(", PI-");
        Keyboard.print(j + 1);
        Keyboard.print(")\n");
        press_count += 1;
      }
    }
  }

  return press_count;
}

/*
   Generic shiftIn function.
*/
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin) {
  uint8_t value = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i) {
    digitalWrite(clockPin, LOW);
    value |= digitalRead(dataPin) << (7 - i);
    digitalWrite(clockPin, HIGH);
  }

  return value;
}

