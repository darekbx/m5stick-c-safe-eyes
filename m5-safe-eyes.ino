/**
 * Simple Safe Eyes implementation for M5Stick-C widget
 * 
 * Short breaks interval: 15 minutes
 * Short breaks duration: 15 seconds
 * Short breaks:
 *  - Tightly close your eyes
 *  - Roll your eyes a few times to each side
 *  - Rotate your eyes in clockwise direction
 *  - Rotate your eyes in counterclockwise direction
 *  - Blink your eyes
 *  - Focus on a point in the far distance
 *  - Have some water
 *  
 * Long breaks interval: 75 minutes
 * Long breaks duration: 60 seconds
 * Long breaks: 
 *  - Walk for a white
 *  - Lean back at your seat and relax
 */

#include <M5StickC.h>

#define SHORT_BREAK_INTERVAL_M 15 // Minutes
#define SHORT_BREAK_TIME_S 15 // Seconds
#define SHORT_BREAKS 7
String shortBreaks[SHORT_BREAKS] = { 
  "Tightly close eyes", 
  "Roll your eyes a few times to each side", 
  "Rotate your eyes in clockwise direction",
  "Rotate your eyes in counterclockwise direction",
  "Blink your eyes", 
  "Focus on a point in the far distance",
  "Have some water"
};

#define LONG_BREAK_INTERVAL_M 75 // Minutes
#define LONG_BREAK_TIME_S 60 // Seconds
#define LONG_BREAKS 2
String longBreaks[LONG_BREAKS] = { 
  "Walk for a white", 
  "Lean back at your seat and relax"
};

RTC_TimeTypeDef TimeStruct;

int loopDelay = 100;
int batteryLevelOffset = 12;
int currentBreakIndex = -1;
int currentLongBreakIndex = -1;
bool isBreakActive = false;
bool isLongBreakActive = false;

static uint16_t color16(uint16_t r, uint16_t g, uint16_t b) {
  uint16_t _color;
  _color = (uint16_t)(r & 0xF8) << 8;
  _color |= (uint16_t)(g & 0xFC) << 3;
  _color |= (uint16_t)(b & 0xF8) >> 3;
  return _color;
}

void setup() {
  M5.begin();
  
  pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
  pinMode(M5_LED, OUTPUT);

  digitalWrite(M5_LED, HIGH);

  setUpTime(0, 0, 0);

  M5.Axp.ScreenBreath(8);
  M5.Lcd.setRotation(3);
}

void loop() {
  M5.Lcd.setTextColor(WHITE);
  
  displayBatteryLevel();
  displayTime();

  handleCancelButton();

  if (TimeStruct.Minutes == LONG_BREAK_INTERVAL_M) {
    currentLongBreakIndex = currentLongBreakIndex + 1;
    if (currentLongBreakIndex >= LONG_BREAKS) {
      currentLongBreakIndex = 0;
    }
    isLongBreakActive = true;
    notifyBreak();
  
  } else if (TimeStruct.Minutes == SHORT_BREAK_INTERVAL_M) {
    currentBreakIndex = currentBreakIndex + 1;
    if (currentBreakIndex >= SHORT_BREAKS) {
      currentBreakIndex = 0;
    }
    isBreakActive = true;
    notifyBreak();
  }

  if (isLongBreakActive) {
    displayBreak(longBreaks[currentLongBreakIndex]);
    if (TimeStruct.Seconds == LONG_BREAK_TIME_S) {
      clearBreak();
    }
  } else if (isBreakActive) {
    displayBreak(shortBreaks[currentBreakIndex]);
    if (TimeStruct.Seconds == SHORT_BREAK_TIME_S) {
      clearBreak();
    }
  }
  
  delay(loopDelay);
}

void displayBreak(String text) {
  M5.Lcd.setCursor(1, 20, 2);
  M5.Lcd.print(text);
}

void notifyBreak() {
  setUpTime(0, 0, 0);
  digitalWrite(M5_LED, LOW);  
}

void setUpTime(int hours, int minutes, int seconds) {
  TimeStruct.Hours   = hours;
  TimeStruct.Minutes = minutes;
  TimeStruct.Seconds = seconds;
  M5.Rtc.SetTime(&TimeStruct);
}

void handleCancelButton() {
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    clearBreak();
  }
}

void displayTime() {
  M5.Lcd.fillRect(122, 0, 160, 12, BLACK);
  M5.Lcd.setCursor(122, 4, 1);
  M5.Rtc.GetTime(&TimeStruct);

  if (isBreakActive) {
    M5.Lcd.setTextColor(GREEN);
  }
  
  M5.Lcd.printf("%02d:%02d", TimeStruct.Minutes, TimeStruct.Seconds);
  M5.Lcd.setTextColor(WHITE);
}

void clearBreak() {
  isBreakActive = false;
  setUpTime(0, 0, 0);
  M5.Lcd.fillRect(0, batteryLevelOffset + 1, 160, 80, BLACK);
  digitalWrite(M5_LED, HIGH);
}
