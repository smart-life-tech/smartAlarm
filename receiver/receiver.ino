int sensitivity = 2500;
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <DS3231.h>
int year = 23;
int month = 05;
int date = 22;
int hour = 16;
int minute = 43;
int second = 00;
int doW = 2; // day of the week to monday
// Set the interval for waking up in seconds
const unsigned long INTERVAL_SECONDS = 60;

RTC_DATA_ATTR int bootCount = 0; // Store boot count in RTC memory

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

DS3231 Clock;
RTClib myRTC;
int Hour;
int Min;

int pset = 15;  // yellow  pushbutton for setting alarm
int phour = 2;  //  blue pushbutton for hour
int pmin = 4;   // greem  pushbutton for minutes
int pexit = 16; // red pushbutton for exit of set alarm
int buzzer = 17;

int h;
int m;
int buttonforset = 0;  // pushbutton state for setting alarm
int buttonforhour = 0; // pushbutton state for hour
int buttonformin = 0;  // pushbutton state for minutes
int buttonforexit = 0; // pushbutton state for exit of set alarm
int activate = 0;
bool ringing = false;
bool ring = true;
// Time t;
int mins, minCount = 0;
int lastMin = 0;
float pressure = 10;
// Init the DS1302
// DS3231  rtc(A4,A5);
// DS1302 rtc(2, 3, 4);

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message
{
  float c;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("pressure: ");
  Serial.println(myData.c);
  pressure = myData.c;
  Serial.println();
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(pset, INPUT_PULLUP);
  pinMode(phour, INPUT_PULLUP);
  pinMode(pmin, INPUT_PULLUP);
  pinMode(pexit, INPUT_PULLUP);
  // Set the clock to run-mode, and disable the write protection
  // Serial.begin(9600);
  Wire.begin();
  // rtc.begin();
  delay(500);
  Serial.println("Nano Ready!");
  // rtc.halt(false);
  //  rtc.writeProtect(false);

  // Setup LCD to 16x2 characters
  lcd.init();
  lcd.backlight();
  // The following lines can be commented out to use the values already stored in the DS1302
  // rtc.setDOW(SATURDAY); // Set Day-of-Week to FRIDAY
  // rtc.setTime(10, 0, 0); // Set the time to 12:00:00 (24hr format)
  // rtc.setDate(11, 11, 2017); // Set the date to August 6th, 2010
  h = EEPROM.read(1);
  m = EEPROM.read(0);
  if (h == 255)
    h = 12;
  if (m == 255)
    m = 30;
  Clock.setClockMode(false);
  Clock.setYear(year);
  Clock.setMonth(month);
  Clock.setDate(date);
  Clock.setDoW(doW);
  Clock.setHour(hour);
  Clock.setMinute(minute);
  Clock.setSecond(second);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
   touchAttachInterrupt(pset, triggerAlarm, 40);
}

void loop()
{
  delay(500);

  // Check if it's time to trigger the alarm based on the interval
  if (bootCount % (INTERVAL_SECONDS / 5) == 0)
  {

    triggerAlarm();
  }
  else
  {
    if (!ringing)
    {
      lcd.noBacklight();
      // Enter deep sleep for the specified interval
      goToSleep(INTERVAL_SECONDS * 1000000); // Convert seconds to microseconds
    }
  }
}

void goToSleep(unsigned long sleepTime)
{
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void triggerAlarm()
{
  // Code to trigger the alarm (e.g., sound a buzzer, send notifications, etc.)
  delay(1000);

  DateTime now = myRTC.now();
  DateTime t = myRTC.now();
  String date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String tim = String(now.hour()) + ":" + String(now.minute());
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
  if (lastMin != now.minute())
  {
    lastMin = now.minute();
    lcd.clear();
  }
  // Serial.print(" since midnight 1/1/1970 = ");
  // Serial.print(now.unixtime());
  // Serial.print("s = ");
  // Serial.print(now.unixtime() / 86400L);
  // Serial.println("d");
  if (activate == 0)
  {

    // Display time on the right conrner upper line
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.setCursor(6, 0);
    lcd.print(tim);

    // Display abbreviated Day-of-Week in the lower left corner
    // lcd.setCursor(0, 1);
    // lcd.print(rtc.getDOWStr(FORMAT_SHORT));

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
  if (buttonforset == LOW)
  {
    activate = 1;
    lcd.clear();
  }
  while (activate == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Set Alarm");
    lcd.setCursor(0, 1);
    lcd.print("Hour= ");
    lcd.setCursor(9, 1);
    lcd.print("Min= ");
    buttonforhour = digitalRead(phour); // set hour for alarm
    if (buttonforhour == LOW)
    {
      delay(500);
      h++;
      lcd.setCursor(5, 1);
      lcd.print(h);
      if (h > 23)
      {
        h = 0;
        lcd.clear();
      }
      delay(100);
      EEPROM.write(1, h);
    }
    buttonformin = digitalRead(pmin); // set minutes for alarm
    if (buttonformin == LOW)
    {
      delay(500);
      m++;
      lcd.setCursor(13, 1);
      lcd.print(m);
      if (m > 59)
      {
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
    if (buttonforexit == LOW)
    {
      activate = 0;
      lcd.clear();
      // Enter deep sleep for the specified interval
      goToSleep(INTERVAL_SECONDS * 1000000); // Convert seconds to microsecond
    }
  }

  if (Hour == h && Min == m)
  {
    Serial.println("now ringing");
    lcd.backlight();
    ringing = true;
    ring = false;
  }
  if (ringing && (pressure > sensitivity))
  {
    if (Min != mins)
    {
      mins = Min;
      minCount++;
    }
    if (minCount >= 15 || !digitalRead(pexit))
    {
      noTone(17);
      minCount = 0;
      ringing = false;
    }
    else if (minCount < 15)
    {
      tone(17, 400, 300);
      Serial.println("ringing");
    } // else if (minCount < 15 && (analogRead(A0) < 500)) {
    // noTone(6);
    // ringing = false; // no ring till next alarm
  }
}