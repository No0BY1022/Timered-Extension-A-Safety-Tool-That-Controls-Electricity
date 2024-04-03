#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Define keypad
const byte ROW_NUM    = 4; //four rows
const byte COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; // connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; // connect to the column pinouts of the keypad
Keypad myKeypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Define LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

// Define relay control pin
const int relayPin = 13; // Change this to the digital pin where your relay is connected

// Variables
int hours = 0;
int minutes = 0;
bool setHours = false;

void setup() {
  lcd.backlight();
  lcd.begin(16,2);  // initialize the lcd 
  lcd.print("Set Time: 00:00");

  // Initialize relay control pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure the relay is initially off
}

void loop() {
  char key = myKeypad.getKey();
  
  if (key) {
    if (key == 'A') {
      startCountdown();
    } else if (key == 'B') {
      setHours = false;
      lcd.clear();
      lcd.print("Set Time (Mins): 00");
    } else if (key == 'C') {
      setHours = true;
      lcd.clear();
      lcd.print("Set Time (Hours): 00");
    } else if (key == 'D') {
      // Handle backspace
      backspace();
    } else if (key == '#') {
      // Reset the timer
      resetTimer();
    } else if ((key >= '0') && (key <= '9')) {
      // Handle numeric input
      updateTimer(key);
    }
  }
}

void backspace() {
  if (setHours) {
    hours = hours / 10;
    lcd.setCursor(13, 1);
    if (hours < 10) {
      lcd.print("0");
    }
    lcd.print(hours);
  } else {
    minutes = minutes / 10;
    lcd.setCursor(10, 1);
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes);
  }
}

void updateTimer(char key) {
  int digit = key - '0';
  if (setHours) {
    hours = hours * 10 + digit;
    hours = constrain(hours, 0, 99);
    lcd.setCursor(13, 1);
    if (hours < 10) {
      lcd.print("0");
    }
    lcd.print(hours);
  } else {
    minutes = minutes * 10 + digit;
    minutes = constrain(minutes, 0, 59);
    lcd.setCursor(10, 1);
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes);
  }
}

void startCountdown() {
  lcd.clear();
  lcd.print("Countdown:");

  // Turn on channel module (relay in this case)
  digitalWrite(relayPin, HIGH);

  while (hours > 0 || minutes > 0) {
    lcd.setCursor(6, 1);
    if (hours < 10) {
      lcd.print("0");
    }
    lcd.print(hours);
    lcd.print(":");
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes);

    delay(60000); // 1 minute delay
    if (minutes > 0) {
      minutes--;
    } else if (hours > 0) {
      hours--;
      minutes = 59;
    }
  }

  // Turn off channel module (relay in this case)
  digitalWrite(relayPin, LOW);

  lcd.clear();
  lcd.print("Time's up!");
}

void resetTimer() {
  hours = 0;
  minutes = 0;
  setHours = false;
  digitalWrite(relayPin, LOW); // Turn off the relay
  lcd.clear();
  lcd.print("Set Time: 00:00");
}
