#include <Adafruit_Fingerprint.h>
#define btnLockPINA 6
#define btnLockPINB 7
#define btnA digitalRead(btnLockPINA)
#define btnB digitalRead(btnLockPINB)

SoftwareSerial mySerial(2, 3);
SoftwareSerial sendSerial(8, 9);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
bool valid = false;
bool match = false;
void setup()
{
  Serial.begin(9600);
  sendSerial.begin(9600);
  pinMode(btnLockPINA, INPUT_PULLUP);
  pinMode(btnLockPINB, INPUT_PULLUP);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop()                     // run over and over again
{
  uint8_t a = getFingerprintID();
  uint8_t b = getFingerprintIDez();
  delay(50);
  
  //Manual
  Serial.print("btnA:"); Serial.println(btnA);
  Serial.print("btnB:"); Serial.println(btnB);
  if (!btnA) {
    Serial.print("btnA:DITEKAN - SENDING");
    sendSerial.write('A');
    delay(7000);
  }
  else if (!btnB) {
    Serial.print("btnB:DITEKAN - SENDING");
    sendSerial.write('B');
    delay(7000);
  }

  //Fingerprint Check
  Serial.print("MYID:"); Serial.print(a);
  Serial.print(" - VALID"); Serial.print(valid);
  Serial.print(" - MATCH"); Serial.println(match);
  if (valid) {
    if (match) {
      Serial.println("SENDING FINGERPRINT");
      if (a == 1 || a == 2)sendSerial.write('A');
      else if (a == 3 || a == 4)sendSerial.write('B');
      delay(7000);
    }
    else {
      Serial.println("SENDING INVALID");
      sendSerial.write('X');
      delay(2000);
    }
  }
  valid = false;
  match = false;
 
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!"); valid = true; match = true;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match"); valid = true; match = false;
    Serial.print("----"); Serial.print(valid); Serial.println(match);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
