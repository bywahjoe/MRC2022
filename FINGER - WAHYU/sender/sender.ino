#include <SoftwareSerial.h>

SoftwareSerial kirim(2, 3);

typedef struct
{
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int g;
  int h;
  int i;
  int j;
} data;
data mydata;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  kirim.begin(9600);
  mydata.a = 1;
  mydata.b = 2;
  mydata.c = 3;
  mydata.d = 4;
  mydata.e = 5;
  mydata.f = 6;
  mydata.g = 7;
  mydata.h = 8;
  mydata.i = 9;
  mydata.j = 10;
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("SENDING BYTES:"); Serial.println(sizeof(mydata));
  Serial.print( mydata.a); Serial.print(" "); Serial.print( mydata.b); Serial.print(" ");
  Serial.print( mydata.c); Serial.print(" "); Serial.print( mydata.d); Serial.print(" ");
  Serial.print( mydata.e); Serial.print(" "); Serial.print( mydata.f); Serial.print(" ");
  Serial.print( mydata.g); Serial.print(" "); Serial.print( mydata.h); Serial.print(" ");
  Serial.print( mydata.i); Serial.print(" "); Serial.println( mydata.j);
  
  kirim.write((byte*)&mydata, sizeof(mydata));
  delay(3000);
  mydata.a = random(1, 10);
  mydata.b = random(11, 20);
  mydata.c = random(21, 30);
  mydata.d = random(31, 40);
  mydata.e = random(41, 50);
  mydata.f = random(51, 60);
  mydata.g = random(61, 70);
  mydata.h = random(71, 80);
  mydata.i = random(81, 90);
  mydata.j = random(91, 100);

}
