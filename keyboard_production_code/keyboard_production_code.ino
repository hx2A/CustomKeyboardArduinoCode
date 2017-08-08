/*
   3/6/2017
   Custom Keyboard Arduino code

   Production code that properly handles key repeats and multiple
   keypresses.

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

int ledBlinkState = 0;

// number of shift registers
int num_registers = 10;
// shift register inputs filled with shiftIn
byte register_states[] = {0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00
                         };
// "memory" of which buttons were pressed for previous read_keyboard()
// loop
byte button_states[] = {0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00
                       };

/*
   List the characters or ascii bytecodes (from "Keyboard.h") for each
   shift register and key, in order. If some of a shift register's
   parallel inputs are not used, set the extra to the number (not
   character) 0.
*/
char keys[] = {
  // REGISTER 0
  KEY_RETURN, KEY_PAGE_DOWN, '?', KEY_DELETE, '=', '/', '*', KEY_PAGE_UP,
  // REGISTER 1
  '4', '7', '5', '8', '9', '+', '-', '6',
  // REGISTER 2
  0, 0, '1', '0', KEY_RETURN, '.', '3', '2',
  // REGISTER 3
  0, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, 'r', 'z', 'q', KEY_LEFT_ARROW,
  // REGISTER 4 (PI-8 is Ctrl-S, Save)
  0, 'g', '(', ')', 'h', 'i', '"', 's',
  // REGISTER 5 (PI-7 is Ctrl-O, Open, PI-8 is Web button)
  'd', 'e', '$', '%', 'f', '&', 'o', 'c',
  // REGISTER 6
  0, 0, 0, 'n', 'o', 'p', 'y', 'x',
  // REGISTER 7
  'u', 'm', 'v', 'w', KEY_LEFT_ALT, 's', 't', ' ',
  // REGISTER 8 (PI-8 is Word button)
  'j', 'k', 'b', 'a', 'l', 'c', '#', 'w',
  // REGISTER 9 (PI-8 is Excel button)
  KEY_LEFT_SHIFT, KEY_CAPS_LOCK, KEY_BACKSPACE, '~', KEY_LEFT_CTRL, '!', '@', 'e'
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
  read_keyboard();
  delay(25);
}

/*
   Interpret keyboard button presses via shift registers and send
   press/release messages to Keyboard API.
*/
void read_keyboard() {
  digitalWrite(latchPin, LOW);

  // read shift registers and store in register_states
  for (int i = 0; i < num_registers; ++i) {
    register_states[i] = shiftIn(dataPin, clockPin);
  }

  digitalWrite(latchPin, HIGH);

  // count button presses to check for hardware failures
  int register_count = check_register_states();

  // now interpret the button states as keypresses
  // don't let the user hit more than 6 buttons at once
  // if that happens it is likely there is a hardware failure
  if (register_count <= 6) {
    bool register_state;
    bool button_state;

    for (int i = 0; i < num_registers; ++i) {
      for (int j = 0; j < 8; ++j) {
        register_state = (1 << j) & register_states[i];
        button_state = (1 << j) & button_states[i];

        if (register_state && !button_state) {
          check_special_handling(i, j, true);
          Keyboard.press(keys[i * 8 + j]);
          button_states[i] |= (1 << j);
        } else if (!register_state && button_state) {
          Keyboard.release(keys[i * 8 + j]);
          check_special_handling(i, j, false);
          button_states[i] ^= (1 << j);
        }
      }
    }
  } else {
    // Blink LED pin to signal hardware failure.
    if (ledBlinkState++ > 10) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    ledBlinkState %= 20;
  }
}

/*
   If any keys need special handling, add extra code here.

   My keyboard has "Open" and "Save" keys, requiring multiple keys to
   be pressed at the same time. It also has special buttons to start
   Excel, Word, and a browser.

   If you don't need extra handling, you can remove this function.
*/
void check_special_handling(int i, int j, bool press) {
  if ((i == 4 && j == 7) || (i == 5 && j == 6)) {
    if (press) {
      Keyboard.press(KEY_LEFT_CTRL);
    } else {
      Keyboard.release(KEY_LEFT_CTRL);
    }
  }
  if ((i == 5 && j == 7) || (i == 8 && j == 7) || (i == 9 && j == 7)) {
    if (press) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ALT);
    } else {
      Keyboard.release(KEY_LEFT_CTRL);
      Keyboard.release(KEY_LEFT_ALT);
    }
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

