#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include "pinku.h"

#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define buttonIN digitalRead(pinButtonIN)
#define buttonSAVE digitalRead(pinButtonSAVE)

String fingerName[3] = {"NULL", "NAURA", "RIZKI" };
String object[8] = {"NULL   ", "DOMPET ", "SELOTIP", "BAJU   ", "MOUSE  ", "GELAS  ", "-DANGER", "NULL   "};
int dangerOBJ = 6;
//HUSKYLENS green line >> Pin 10; blue line >> Pin 11
SoftwareSerial mySerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoLock1;
Servo servoLock2;
HUSKYLENS huskylens;
void printResult(HUSKYLENSResult result);

//Config Servo
int servoOpen = 0, servoClose = 180;

typedef struct
{
  int32_t lockerA_A;
  int32_t lockerA_B;
  int32_t lockerA_C;
  int32_t lockerA_D;
  int32_t lockerA_E;

  int32_t lockerB_A;
  int32_t lockerB_B;
  int32_t lockerB_C;
  int32_t lockerB_D;
  int32_t lockerB_E;
  int32_t loginID;

} items;
items mylocker;
int sizex = sizeof(mylocker);
void setup() {
  // put your setup code here, to run once:
  mylocker.lockerA_A = 0;
  mylocker.lockerA_B = 0;
  mylocker.lockerA_C = 0;
  mylocker.lockerA_D = 0;
  mylocker.lockerA_E = 0;

  mylocker.lockerB_A = 0;
  mylocker.lockerB_B = 0;
  mylocker.lockerB_C = 0;
  mylocker.lockerB_D = 0;
  mylocker.lockerB_E = 0;
  mylocker.loginID = 0;
  Serial.print("SIZE STRUCT-"); Serial.println(sizex);

  Serial.begin(9600);
  Serial2.begin(9600);//Recv Finger
  Serial3.begin(9600);//Send to ESP
  mySerial.begin(9600);//Huskyy

  pinMode(buzz, OUTPUT);
  pinMode(relayIN1, OUTPUT);
  pinMode(relayIN2, OUTPUT);
  pinMode(pinButtonIN, INPUT_PULLUP);
  pinMode(pinButtonSAVE, INPUT_PULLUP);
  servoLock1.attach(pinServoLocker1);
  servoLock2.attach(pinServoLocker2);
  servoCLOSE(1);
  servoCLOSE(2);
  buzzOFF();

  while (!huskylens.begin(mySerial))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }



  lcd.init();
  lcd.backlight();
  welcomeLCD();
  waitDisplay();
  //  fingerDisplay(1);
  //  while (1);

  //  while(1);
}
void loop() {
  waitDisplay();
  Serial.print(!buttonIN); Serial.println(!buttonSAVE);
  if (Serial2.available()) {
    char recv = Serial2.read();
    Serial.println(recv);

    if (recv == 'A') {
      mylocker.loginID = 1;
      fingerDisplay(1);
      lockerOPEN(1);
      mainMenu(1);
      lockerCLOSE(1);
      Serial2.begin(9600);

    }
    else if (recv == 'B') {
      mylocker.loginID = 2;
      fingerDisplay(2);
      lockerOPEN(2);
      mainMenu(2);
      lockerCLOSE(2);
      Serial2.begin(9600);
    }
    else if (recv == 'X') {
      fingerInvalid();
    }
    else {}
    lcd.clear();
  };
  // put your main code here, to run repeatedly:
  //  relayON1();
  //  relayON2();
  //  delay(3000);
  //  relayOFF1();
  //  relayOFF2();
  //  delay(3000);
}
void displayItem(int select) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (select == 1) {//Display Fingerprint locker1
    lcd.print("A1 ");
    lcd.print("A2 ");
    lcd.print("A3 ");
    lcd.print("A4 ");
    lcd.print("A5");
    lcd.setCursor(0, 1); lcd.print(mylocker.lockerA_A);
    lcd.setCursor(3, 1); lcd.print(mylocker.lockerA_B);
    lcd.setCursor(6, 1); lcd.print(mylocker.lockerA_C);
    lcd.setCursor(9, 1); lcd.print(mylocker.lockerA_D);
    lcd.setCursor(12, 1); lcd.print(mylocker.lockerA_E);
  }
  else {  //Display Fingerprint locker2
    lcd.print("B1 ");
    lcd.print("B2 ");
    lcd.print("B3 ");
    lcd.print("B4 ");
    lcd.print("B5");
    lcd.setCursor(0, 1); lcd.print(mylocker.lockerB_A);
    lcd.setCursor(3, 1); lcd.print(mylocker.lockerB_B);
    lcd.setCursor(6, 1); lcd.print(mylocker.lockerB_C);
    lcd.setCursor(9, 1); lcd.print(mylocker.lockerB_D);
    lcd.setCursor(12, 1); lcd.print(mylocker.lockerB_E);
  }
}
void addItem(int objectID, int userID) {
  //Counter Item
  if (userID == 1) {
    if (objectID == 1)mylocker.lockerA_A = mylocker.lockerA_A + 1;
    else if (objectID == 2)mylocker.lockerA_B = mylocker.lockerA_B + 1;
    else if (objectID == 3)mylocker.lockerA_C = mylocker.lockerA_C + 1;
    else if (objectID == 4)mylocker.lockerA_D = mylocker.lockerA_D + 1;
    else if (objectID == 5)mylocker.lockerA_E = mylocker.lockerA_E + 1;
  }
  else {
    if (objectID == 1)mylocker.lockerB_A = mylocker.lockerB_A + 1;
    else if (objectID == 2)mylocker.lockerB_B = mylocker.lockerB_B + 1;
    else if (objectID == 3)mylocker.lockerB_C = mylocker.lockerB_C + 1;
    else if (objectID == 4)mylocker.lockerB_D = mylocker.lockerB_D + 1;
    else if (objectID == 5)mylocker.lockerB_E = mylocker.lockerB_E + 1;

  }

}
void mainMenu(int select) {
  int objectID  = 0;
  lcd.clear();

  while (1)
  {
    lcd.setCursor(0, 0);
    lcd.print("HUSKY:");
    lcd.print(object[objectID]);
    lcd.setCursor(0, 1);
    lcd.print("ID:");
    lcd.print(objectID);
    lcd.setCursor(5, 1);
    lcd.print("IS-VALID??");

    //Danger OBJ
    if (objectID == dangerOBJ) {
      buzzON();
      delay(1000);
    }
    else buzzOFF();

    //Husky
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if (!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if (!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else
    {
      Serial.println(F("###########"));
      while (huskylens.available())
      {
        HUSKYLENSResult result = huskylens.read();
        printResult(result);
        objectID = result.ID;
        Serial.print (objectID); Serial.println("------");
      }

    }

    if (!buttonIN) {
      addItem(objectID, select);
      displayItem(select);
      delay(3000);
      lcd.clear();

      //COUNTER
    }
    if (!buttonSAVE) {
      buzzOFF();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SAVING ITEMS");
      delay(2000);
      //SAVE AND EXIT
      displayItem(select);
      Serial3.write((byte*)&mylocker, sizeof(mylocker));
      delay(5000);
      break;
    }

  }

}
void welcomeLCD() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("DEVICE");
  lcd.setCursor(4, 1);
  lcd.print("IS_START");
  delay(2000);
  lcd.clear();
}
void waitDisplay() {
  //  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WAITING");
  lcd.setCursor(2, 1);
  lcd.print("FINGERPRINT");
  //  delay(500);
}
void fingerDisplay(int id) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME!");
  lcd.setCursor(13, 0);
  
  if(id==1)lcd.print("[A]");
  else lcd.print("[B]");
  
  lcd.setCursor(0, 1);
  lcd.print(fingerName[id]);
  delay(4000);
}
void fingerInvalid() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FINGERPRINT");
  lcd.setCursor(0, 1);
  lcd.print("INVALID");
  delay(750);
  lcd.clear();
}
void buzzON() {
  digitalWrite(buzz, LOW);
}
void buzzOFF() {
  digitalWrite(buzz, HIGH);
}
void relayON(int select) {
  if (select == 1)digitalWrite(relayIN1, HIGH);
  else digitalWrite(relayIN2, HIGH);
}
void relayOFF(int select) {
  if (select == 1)digitalWrite(relayIN1, LOW);
  else digitalWrite(relayIN2, LOW);
}
void servoOPEN(int select) {
  if (select == 1)servoLock1.write(servoOpen);
  else servoLock2.write(servoOpen);
}
void servoCLOSE(int select) {
  if (select == 1)servoLock1.write(servoClose);
  else servoLock2.write(servoClose);
}
void lockerOPEN(int select) {
  relayON(select);
  delay(2500);
  servoOPEN(select);
}
void lockerCLOSE(int select) {
  servoCLOSE(select);
  delay(2500);
  relayOFF(select);

}
void printResult(HUSKYLENSResult result) {
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
  }
  else if (result.command == COMMAND_RETURN_ARROW) {
    Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
  }
  else {
    Serial.println("Object unknown!");
  }
}
