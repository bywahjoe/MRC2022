#include "pinku.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"

//Mode Selected
#define btnAuto digitalRead(autox)
#define btnManual1 digitalRead(manual1)
#define btnManual2 digitalRead(manual2)

RTC_DS3231 rtc;
DHT dht(pin_suhu, type_suhu);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

bool statusFan, statusLamp, statusMtr, isNight;
bool isWindowsOpen;
bool useRTC = true;

float statusTemp;
String viewStatus(bool val, bool isMotorMode = false);
int date[3];
int times[3];

//Config
float hot = 30.00;
int servoOpen = 0, servoClose = 90;
int timeWindows=7500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Selected Mode
  pinMode(autox, INPUT_PULLUP);
  pinMode(manual1, INPUT_PULLUP);
  pinMode(manual2, INPUT_PULLUP);

  pinMode(pirpin, INPUT);
  pinMode(flamepin, INPUT);
  pinMode(ldrpin, INPUT);
  pinMode(ledpin, OUTPUT);
  pinMode(buzzpin, OUTPUT);
  pinMode(fanpin, OUTPUT);

  rtc.begin();
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  myservo.attach(servopin);
  dht.begin();
  runDefault();

  //LCD
  lcd.init();
  lcd.backlight();
  Serial.println(isDark());
  //  fanON();
  welcomeLCD();
  fanOFF();

  //  windowsClose();
  //  windowsOpen();
  //  while(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  statusTemp = getTemp();
  DateTime now = rtc.now();
  date[0] = now.day();
  date[1] = now.month();
  date[2] = now.year();
  times[0] = now.hour();
  times[1] = now.minute();
  times[2] = now.second();

  if (useRTC) {
    if (times[0] >= 7 && times[0] <= 15) isNight = false;
    else isNight = true;
  };

  //Print Date
  for (int i; i < 3; i++) {
    Serial.print(date[i]);
    if (i == 2)Serial.println();
    else Serial.print('/');
  }
  //Print Time
  for (int i; i < 3; i++) {
    Serial.print(times[i]);
    if (i == 2)Serial.println();
    else Serial.print(':');
  }

  viewLCD();
  if (isNight) {
    nightMode();
  } else {
    normalMode();
  }

  

  if (!btnAuto) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AUTO MODE:");
    lcd.setCursor(0, 1);
    lcd.print("byRTC");
    useRTC = true;
    delay(2500);
    lcd.clear();
  }
  if (!btnManual1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MANUAL MODE:");
    lcd.setCursor(0, 1);
    lcd.print("PAGI-SORE");
    useRTC = false;
    isNight = false;
    delay(2500);
    lcd.clear();
  }
  if (!btnManual2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MANUAL MODE:");
    lcd.setCursor(0, 1);
    lcd.print("MALAM");
    useRTC = false;
    isNight = true;
    delay(2500);
    lcd.clear();
  }
  // is Room in Danger
  if (isDanger()) {
    buzzON();
  } else {
    buzzOFF();
  }
  
  //DEBUG
  Serial.print("LDR:"); Serial.println(isDark());
  Serial.print("PIR:"); Serial.println(isPerson());
  Serial.print("USERTC-SIANG-MALAM-");
  Serial.print(!btnAuto); Serial.print(!btnManual1); Serial.println(!btnManual2);
}
void welcomeLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  lcd.setCursor(0, 1);
  lcd.print("Smart Classroom");
  delay(2000);
  lcd.clear();

}
void doorOpen() {
  lcd.clear();
  myservo.write(servoOpen);
  lcd.print("Door Open 5s");
  delay(5000);
  doorClose();
  lcd.clear();

}
void doorClose() {
  myservo.write(servoClose);
}
void ledON() {
  digitalWrite(ledpin, HIGH);
  statusLamp = true;
}
void ledOFF() {
  digitalWrite(ledpin, LOW);
  statusLamp = false;
}
void buzzON() {
  digitalWrite(buzzpin, HIGH);
}
void buzzOFF() {
  digitalWrite(buzzpin, LOW);
}
void fanON() {
  statusFan = true;
 analogWrite(fanpin, 60);
}
void fanOFF() {
  statusFan = false;
  analogWrite(fanpin, 0);
}
void windowsStop() {
  digitalWrite(mtrIN1, LOW);
  digitalWrite(mtrIN2, LOW);
}
void windowsClose() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Warning:");
  lcd.setCursor(0, 1);
  lcd.print("WindowsIsClose");

  digitalWrite(mtrIN1, HIGH);
  digitalWrite(mtrIN2, LOW);
  delay(timeWindows);
  windowsStop();
  isWindowsOpen = false;

  lcd.clear();
}
void windowsOpen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Warning:");
  lcd.setCursor(0, 1);
  lcd.print("WindowsIsOpen");

  digitalWrite(mtrIN1, LOW);
  digitalWrite(mtrIN2, HIGH);
  delay(timeWindows);
  windowsStop();
  isWindowsOpen = true;

  lcd.clear();
}

bool isDark() {
  return digitalRead(ldrpin);
}
bool isDanger() {
  return digitalRead(flamepin);
}
bool isPerson() {
  return digitalRead(pirpin);
}
void runDefault() {
  doorClose();
  ledOFF();
  buzzOFF();
  fanOFF();

}
String viewStatus(bool val, bool isMotorMode) {
  if (isMotorMode) {
    if (val)return "OPN";
    else return "CLS";
  }
  if (val) return "ON ";
  else return "OFF";
}
float getTemp() {
  float val = dht.readTemperature();
  return val;
}
void viewLCD() {

  lcd.setCursor(0, 0);
  lcd.print("FAN :");
  lcd.print(viewStatus(statusFan));

  lcd.setCursor(0, 1);
  lcd.print("LAMP:");
  lcd.print(viewStatus(statusLamp));

  lcd.setCursor(9, 0);
  lcd.print(statusTemp);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(9, 1);
  lcd.print("WND:");
  lcd.print(viewStatus(statusMtr, 1));
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DATE :");
  lcd.print(date[0]); lcd.print("/");
  lcd.print(date[1]); lcd.print("/");
  lcd.print(date[2]);

  lcd.setCursor(0, 1);
  String mode = "-NORMAL";
  if (isNight)mode = "-NIGHT";
  lcd.print(times[0]); lcd.print(":");
  lcd.print(times[1]); lcd.print(":");
  lcd.print(times[2]); lcd.print(" ");
  lcd.print(mode);
  delay(1000);
  lcd.clear();

}
void nightMode() {
  if (isWindowsOpen)windowsClose();
  ledON();

  doorClose();
//  buzzOFF();
  fanOFF();
}
void normalMode() {
  if (!isWindowsOpen)windowsOpen();

  //Room is Dark
  if (isDark()) {
    ledON();
    statusLamp = true;
  } else {
    ledOFF();
    statusLamp = false;
  }

  //Pir Detected
  if (isPerson()) {
    doorOpen();
  } else {
    doorClose();
  }

  //is Room is Hot
    if ( statusTemp > hot)fanON();
    else fanOFF();


}
