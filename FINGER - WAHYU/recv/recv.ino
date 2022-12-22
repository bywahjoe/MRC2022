#include <SoftwareSerial.h>

SoftwareSerial terima(2, 3);

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

//struct data mydata;
data mydata;
data mydata1;
int sizex = sizeof(data);
//data PtrstructTable;
//PtrstructTable = &data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  terima.begin(9600);
  Serial.print("STRUCT SIZE:"); Serial.println(sizex);
}

void loop() {
  if (terima.available()) {
    //    Serial.readBytes((char*)mydata, sizex);
    receive(&mydata);
    Serial.println("RECEIVED:");
    Serial.print( mydata.a); Serial.print(" "); Serial.print( mydata.b); Serial.print(" ");
    Serial.print( mydata.c); Serial.print(" "); Serial.print( mydata.d); Serial.print(" ");
    Serial.print( mydata.e); Serial.print(" "); Serial.print( mydata.f); Serial.print(" ");
    Serial.print( mydata.g); Serial.print(" "); Serial.print( mydata.h); Serial.print(" ");
    Serial.print( mydata.i); Serial.print(" "); Serial.println( mydata.j);

  }
  // put your main code here, to run repeatedly:

}
void receive(data* table)
{
  terima.readBytes((char*)table, sizex);

}
