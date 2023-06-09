int  year = 23;
int month = 03;
int date = 15;
int hour = 19;
int minute = 53;
int second = 00;
int doW = 2; //day of the week to monday

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <DS3231.h>
DS3231 Clock;
RTClib myRTC;
int Hour;
int Min;
int pset = 8; //yellow  pushbutton for setting alarm
int phour = 9; //  blue pushbutton for hour
int pmin = 10; //greem  pushbutton for minutes
int pexit = 11; // red pushbutton for exit of set alarm
int buzzer = 6;
int h;
int m;
int buttonforset = 0; // pushbutton state for setting alarm
int buttonforhour = 0; // pushbutton state for hour
int buttonformin = 0;// pushbutton state for minutes
int buttonforexit = 0; // pushbutton state for exit of set alarm
int activate = 0;
bool ringing = false;
bool ring = true;
//Time t;
int mins, minCount = 0;
int lastMin = 0;
// Init the DS1302
//DS3231  rtc(A4,A5);
//DS1302 rtc(2, 3, 4);

void setup()
{
  pinMode(pset, INPUT_PULLUP);
  pinMode(phour, INPUT_PULLUP);
  pinMode(pmin, INPUT_PULLUP);
  pinMode(pexit, INPUT_PULLUP);
  // Set the clock to run-mode, and disable the write protection
  Serial.begin(9600);
  Wire.begin();
  //rtc.begin();
  delay(500);
  Serial.println("Nano Ready!");
  // rtc.halt(false);
  //  rtc.writeProtect(false);

  // Setup LCD to 16x2 characters
  lcd.init();
  lcd.backlight();
  // The following lines can be commented out to use the values already stored in the DS1302
  //rtc.setDOW(SATURDAY); // Set Day-of-Week to FRIDAY
  //rtc.setTime(10, 0, 0); // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(11, 11, 2017); // Set the date to August 6th, 2010
  h = EEPROM.read(1);
  m = EEPROM.read(0);
  if (h == 255)h = 12;
  if (m == 255)m = 30;
  Clock.setYear(year);
  Clock.setMonth(month);
  Clock.setDate(date);
  Clock.setDoW(doW);
  Clock.setHour(hour);
  Clock.setMinute(minute);
  Clock.setSecond(second);
}

void loop()
{ delay(1000);

  DateTime now = myRTC.now();
  DateTime t = myRTC.now();
  String date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String tim = String(now.hour()) + ":" + String(now.minute()) ;
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  if (lastMin != now.minute()) {
    lastMin = now.minute();
    lcd.clear();
  }
  // Serial.print(" since midnight 1/1/1970 = ");
  // Serial.print(now.unixtime());
  // Serial.print("s = ");
  // Serial.print(now.unixtime() / 86400L);
  // Serial.println("d");
  if (activate == 0) {

    // Display time on the right conrner upper line
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.setCursor(6, 0);
    lcd.print(tim);

    // Display abbreviated Day-of-Week in the lower left corner
    //lcd.setCursor(0, 1);
    //lcd.print(rtc.getDOWStr(FORMAT_SHORT));

    // Display date in the lower right corner
    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    lcd.setCursor(6, 1);
    lcd.print(date);
    //  t = rtc.getTime();
    Hour = t.hour();
    Min = t.minute();
    buttonforset = digitalRead(pset);
  } // setting button pressed
  if (buttonforset == LOW) {
    activate = 1;
    lcd.clear();
  }
  while (activate == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Set Alarm");
    lcd.setCursor(0, 1);
    lcd.print("Hour= ");
    lcd.setCursor(9, 1);
    lcd.print("Min= ");
    buttonforhour = digitalRead(phour); // set hour for alarm
    if (buttonforhour == LOW) {
      delay(500);
      h++;
      lcd.setCursor(5, 1);
      lcd.print(h);
      if (h > 23) {
        h = 0;
        lcd.clear();
      }
      delay(100);
      EEPROM.write(1, h);
    }
    buttonformin = digitalRead(pmin); // set minutes for alarm
    if (buttonformin == LOW) {
      delay(500);
      m++;
      lcd.setCursor(13, 1);
      lcd.print(m);
      if (m > 59) {
        m = 0;
        lcd.clear();
      }
      delay(100);
      EEPROM.write(0, m);
    }

    lcd.setCursor(5, 1);
    lcd.print(h);
    lcd.setCursor(13, 1);
    lcd.print(m);
    buttonforexit = digitalRead(pexit); // exit from set alarm mode
    if (buttonforexit == LOW) {
      activate = 0;
      lcd.clear();
    }
  }

  if (Hour == h && Min == m ) {
    Serial.println("now ringing");
    ringing = true;
    ring = false;
  }
  if (ringing && (analogRead(A0) > 500)) {
    if (Min != mins) {
      mins = Min;
      minCount++;
    }
    if (minCount >= 15) {
      noTone(6);
    } else if (minCount < 15) {
      tone(6, 400, 300);
      Serial.println("ringing");
    } //else if (minCount < 15 && (analogRead(A0) < 500)) {
    //noTone(6);
    //ringing = false; // no ring till next alarm
  }
  delay (500);
}

