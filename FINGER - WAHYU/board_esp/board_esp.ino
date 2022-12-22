#include "CTBot.h"
#define chatID -894806295
#define chatIDA -515049703
#define chatIDB -869389890
#define led 4

#define buzz 26
CTBot myBot;

////WIFI CONFIG
//String ssid = "USER_6CB010";
//String pass = "00000000";

//WIFI CONFIG
String ssid = "robotku";
String pass = "robot1234";

//BOT TOKEN
String token = "2071469876:AAHc9U7cfd3jV8KCwX2_tjH7aJFG3J8JvHE";
String fingerName[3] = {"NULL", "NAURA", "RIZKI" };

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
int sizex = sizeof(items);
void kirim(String pesan = "TEST", int target = 0);
void setup() {
  //Default
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


  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

  pinMode(buzz, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
//  while(1);
  for (int i = 0; i < 3; i++) {
    digitalWrite(led, HIGH);
    delay(250);
    digitalWrite(led, LOW);
    delay(250);
  }

  Serial.print("SIZE STRUCT-"); Serial.print(sizex);
  Serial.println("\nSTART TELEGRAM -- CONECT WIFI");

  myBot.wifiConnect(ssid, pass); //WIFI CONNECT
  myBot.setTelegramToken(token); //TOKEN SETT

  //TEST KONEKSI
  if (myBot.testConnection()) {
    Serial.println("\nSIGNAL OK");
  }
  else {
    Serial.println("\nERROR NO SIGNAL");
    digitalWrite(led, HIGH);
  }
  firstMessage();
  sendStatusLocker();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    recv(&mylocker);
    //RECEIVE A
    Serial.print("RECEIVED-A:");
    Serial.print( mylocker.lockerA_A); Serial.print(" "); Serial.print( mylocker.lockerA_B); Serial.print(" ");
    Serial.print( mylocker.lockerA_C); Serial.print(" "); Serial.print( mylocker.lockerA_D); Serial.print(" ");
    Serial.println( mylocker.lockerA_E);
    //RECEIVE B
    Serial.print("RECEIVED-B:");
    Serial.print( mylocker.lockerB_A); Serial.print(" "); Serial.print( mylocker.lockerB_B); Serial.print(" ");
    Serial.print( mylocker.lockerB_C); Serial.print(" "); Serial.print( mylocker.lockerB_D); Serial.print(" ");
    Serial.println( mylocker.lockerB_E);

    sendStatusLocker();
  }
}
void recv(items* table)
{
  Serial2.readBytes((char*)table, sizex);

}
void kirim(String pesan, int target) {
  digitalWrite(buzz, HIGH);
  if (target == 0)myBot.sendMessage(chatID, pesan);
  else if (target == 1)myBot.sendMessage(chatIDA, pesan);
  else if (target == 2)myBot.sendMessage(chatIDB, pesan);
  delay(500);
  digitalWrite(buzz, LOW);
}
void firstMessage() {
  String msg;
  msg = "SMARTLOCKER STARTED!!";

  kirim(msg);
}
String msgA() {
  String msg;
  msg += "#Locker1 - " + fingerName[1] + "\n";
  msg += String(mylocker.lockerA_A) + "x DOMPET \n";
  msg += String(mylocker.lockerA_B) + "x SELOTIP \n";
  msg += String(mylocker.lockerA_C) + "x BAJU \n";
  msg += String(mylocker.lockerA_D) + "x MOUSE \n";
  msg += String(mylocker.lockerA_E) + "x GELAS \n\n";
  return msg;
}
String msgB() {
  String msg;
  msg += "#Locker2 - " + fingerName[2] + "\n";
  msg += String(mylocker.lockerB_A) + "x DOMPET \n";
  msg += String(mylocker.lockerB_B) + "x SELOTIP \n";
  msg += String(mylocker.lockerB_C) + "x BAJU \n";
  msg += String(mylocker.lockerB_D) + "x MOUSE \n";
  msg += String(mylocker.lockerB_E) + "x GELAS \n";
  return msg;
}
String msgAll() {
  String msg = msgA() + msgB();
  return msg;
}

void sendStatusLocker() {
  String msg;

  msg = "SmartLocker Alert!!\n";
  if (mylocker.loginID == 0)msg += msgAll();
  else if (mylocker.loginID == 1)msg += msgA();
  else if (mylocker.loginID == 2)msg += msgB();
  msg += "+---+---+---+---+---+---+---+---+---+";

  kirim(msg, mylocker.loginID);
}
