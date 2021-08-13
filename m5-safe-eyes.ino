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

#define BREAK_INTERVAL 900 // Seconds 15 mins
#define SHORT_BREAK_TIME 15 // Seconds
#define LONG_BREAK_TIME 60 // Seconds
#define BREAK_BLINK_DELAY 500 // Milliseconds
#define BREAKS 9

String breakMessages[BREAKS] = { 
  "Tightly close eyes", 
  "Roll your eyes a few times to each side", 
  "Rotate your eyes in clockwise direction",
  "Rotate your eyes in counterclockwise direction",
  "Walk for a white", 
  "Blink your eyes", 
  "Focus on a point in the far distance",
  "Have some water",
  "Lean back at your seat and relax"
};

byte longBreakIndex[2] = {
  4, 8
};

RTC_TimeTypeDef TimeStruct;

int loopDelay = 100;
int batteryLevelOffset = 12;
int currentBreakIndex = -1;
bool isBreakActive = false;
bool isLongBreakActive = false;
bool breakBlinkFlag = false;

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

  if (isBreakActive) {
    blikDuringBreak();
  } 
    
  if (TimeStruct.Minutes >= (BREAK_INTERVAL / 60)) {
    currentBreakIndex = currentBreakIndex + 1;
    if (currentBreakIndex >= BREAKS) {
      currentBreakIndex = 0;
    }

    for (int i = 0; i < 2; i++) {
      if (currentBreakIndex == longBreakIndex[i]) {
        isLongBreakActive = true;
        break;
      }
    }

    if (!isLongBreakActive) {
      isBreakActive = true;
    }
    
    notifyBreak();
  }

  if (isBreakActive) {
    displayBreak(breakMessages[currentBreakIndex]);
    if (TimeStruct.Seconds >= SHORT_BREAK_TIME) {
      clearBreak();
    }
  }
  if (isLongBreakActive) {
    displayLongBreak(breakMessages[currentBreakIndex]);
    if (TimeStruct.Minutes >= (LONG_BREAK_TIME / 60)) {
      clearBreak();
    }
  }
  
  delay(loopDelay);
}

void blikDuringBreak() {
  if (breakBlinkFlag) {
    digitalWrite(M5_LED, LOW);
  } else {
    digitalWrite(M5_LED, HIGH);
  }
  delay(BREAK_BLINK_DELAY);
  breakBlinkFlag = !breakBlinkFlag;  
}

void displayBreak(String text) {
  M5.Lcd.setCursor(1, 20, 2);
  M5.Lcd.print(text);
}

void displayLongBreak(String text) {
  M5.Lcd.setCursor(1, 20, 2);
  M5.Lcd.print(text);
  M5.Lcd.setCursor(40, 60, 2);
  M5.Lcd.print("(long break)");
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

  if (isBreakActive || isLongBreakActive) {
    M5.Lcd.setTextColor(GREEN);
  }
  
  M5.Lcd.printf("%02d:%02d", TimeStruct.Minutes, TimeStruct.Seconds);
  M5.Lcd.setTextColor(WHITE);
}

void clearBreak() {
  isBreakActive = false;
  isLongBreakActive = false;
  setUpTime(0, 0, 0);
  M5.Lcd.fillRect(0, batteryLevelOffset + 1, 160, 80, BLACK);
  digitalWrite(M5_LED, HIGH);
}
