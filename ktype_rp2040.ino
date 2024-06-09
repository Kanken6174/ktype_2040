#include <Arduino.h>
#include <Keyboard.h>

// Define the column and row pins
const uint8_t colPins[] = {8, 9, 10, 11, 12, 13, 14, 15, 6};
const uint8_t rowPins[] = {A1, A2, A3, 21, 20, 19, 18, 17, 16, 7};

// Keymap for translating row/col to HID keycodes
const uint8_t keyMap[10][9][2] = {
    // Row 0
    {{KEY_UP_ARROW, 0}, {KEY_RIGHT_ARROW, 0}, {KEY_DOWN_ARROW, 0}, {KEY_KP_9, 0}, {KEY_KP_6, 0}, {KEY_KP_0, 0}, {KEY_KP_ENTER, 0}, {0, 0}, {0, 0}},
    // Row 1
    {{KEY_LEFT_ARROW, 0}, {'*', 0}, {KEY_KP_7, 0}, {KEY_KP_8, 0}, {'5', '%'}, {'1', '!'}, {'2', '"'}, {'0', '_'}, {'.', 0}},
    // Row 2
    {{KEY_PAGE_UP, 0}, {']', 0}, {KEY_KP_ENTER, 0}, {'[', 0}, {KEY_KP_4, 0}, {KEY_RIGHT_SHIFT, 0}, {'\\', 0}, {KEY_RIGHT_CTRL, 0}, {0, 0}},
    // Row 3
    {{'0', '_'}, {'9', ')'}, {'o', 0}, {'i', 0}, {'l', 0}, {'k', 0}, {'m', 0}, {',', 0}, {KEY_NUM_LOCK, 0}},
    // Row 4
    {{'p', 0}, {'@', 0}, {KEY_KP_ENTER, 0}, {'p', 0}, {';', 0}, {'.', 0}, {'/', 0}, {'.', 0}, {0, 0}},
    // Row 5
    {{'8', '('}, {'7', '\''}, {'u', 0}, {'y', 0}, {'h', 0}, {'j', 0}, {'n', 0}, {' ', 0}, {0, 0}},
    // Row 6
    {{'4', '$'}, {'3', '#'}, {'e', 0}, {'w', 0}, {'s', 0}, {'d', 0}, {'c', 0}, {'x', 0}, {0, 0}},
    // Row 7
    {{'6', '&'}, {'5', '%'}, {'r', 0}, {'t', 0}, {'g', 0}, {'f', 0}, {'b', 0}, {'v', 0}, {0, 0}},
    // Row 8
    {{'1', '!'}, {'2', '"'}, {KEY_ESC, 0}, {'q', 0}, {KEY_TAB, 0}, {'a', 0}, {KEY_CAPS_LOCK, 0}, {'z', 0}, {0, 0}},
    // Row 9
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {KEY_BACKSPACE, 0}}
};

bool keyState[10][9] = {0};  // Store the state of each key (pressed or not)
bool capsLockState = false;
bool shiftState = false;

void setup() {
  // Initialize columns as outputs and set them HIGH
  for (uint8_t col = 0; col < sizeof(colPins); col++) {
    pinMode(colPins[col], OUTPUT);
    digitalWrite(colPins[col], HIGH);
  }

  // Initialize rows as inputs with pull-up resistors
  for (uint8_t row = 0; row < sizeof(rowPins); row++) {
    pinMode(rowPins[row], INPUT_PULLUP);
  }

  Keyboard.begin();
}

void loop() {
  // Scan the matrix
  for (uint8_t col = 0; col < sizeof(colPins); col++) {
    // Activate the current column
    digitalWrite(colPins[col], LOW);
    delay(1);

    // Check each row
    for (uint8_t row = 0; row < sizeof(rowPins); row++) {
      bool currentState = (digitalRead(rowPins[row]) == LOW);
      if (currentState != keyState[row][col]) {  // If the state has changed
        keyState[row][col] = currentState;
        uint8_t normalKeycode = keyMap[row][col][0];
        uint8_t capsKeycode = keyMap[row][col][1];
        uint8_t keycode = (shiftState && capsKeycode != 0) ? capsKeycode : (capsLockState && capsKeycode != 0) ? capsKeycode : normalKeycode;

        if (keycode != 0) {
          if (currentState) {
            if (normalKeycode == KEY_CAPS_LOCK) {
              capsLockState = !capsLockState;
            } else if (normalKeycode == KEY_RIGHT_SHIFT) {
              shiftState = true;
              Keyboard.press(keycode);
            } else {
              Keyboard.press(keycode);  // Key pressed
            }
          } else {
            if (normalKeycode == KEY_RIGHT_SHIFT) {
              shiftState = false;
              Keyboard.release(keycode);
            } else {
              Keyboard.release(keycode);  // Key released
            }
          }
        }
      }
    }

    // Deactivate the current column
    digitalWrite(colPins[col], HIGH);
    delay(1);
  }
}
