#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// RX & TX
SoftwareSerial komunikasi (5, 6);


float calibration = 21.40; //change this value to calibrate
const int analogInPin = A0;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;

//kalibrasi
//int ph = 0;

//Parsing Serial
String sData, data[10];
boolean parsing = false;

//data sementara
int dataSementara = 6;

//Rellay Asam Dan Basah
int asam = 2;
int basah = 3;

int kalibrasi() {
  int phValue;
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(analogInPin);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
    float pHVol = (int)avgValue * 5.0 / 1024 / 6;
    phValue = -5.68 * pHVol + calibration;
  }
  Serial.print("PH = ");
  Serial.println(phValue);

  return phValue;
}

int kalibrasi2() {
  int phValue;
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(analogInPin);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
    float pHVol = (int)avgValue * 5.0 / 1024 / 6;
    phValue = -5.68 * pHVol + calibration;
  }

  return phValue;
}

void gerak(int pin, int data) {
  while (kalibrasi() != data) {
    digitalWrite(pin, HIGH);
  }
  digitalWrite(pin, LOW);
}

void cek() {
  int ph = kalibrasi();
  int ph_minimum = "";
  int ph_maximum = "";

  while (komunikasi.available()) {
    char inChar = komunikasi.read();
    //inChar.trim();
    sData += inChar;
    if (inChar == '$') {
      parsing = true;
    }
    if (parsing) {
      int q = 0;
      for (int i = 0; i < sData.length(); i++) {
        if (sData[i] == '#') {
          q++;
          data[q] = "";
        } else {
          data[q] += sData[i];
        }
      }
      Serial.println("Data Masuk :" + sData);

      ph_minimum = data[1].toInt();
      ph_maximum = data[2].toInt();

      
         if (ph_minimum <= ph && ph_maximum >= ph) {
           Serial.print(" PH Kolam = ");
           Serial.println(ph);
           digitalWrite(basah, HIGH);
           digitalWrite(asam, HIGH);
           Serial.print(" Status = ");
           Serial.println("Normal");
         } else if (ph_maximum > ph) {
           Serial.print(" PH Kolam = ");
           Serial.println(ph);
           digitalWrite(basah, LOW);
           digitalWrite(asam, HIGH);
           Serial.print(" Status = ");
           Serial.println(" Poma Air Basah Menyala. ");
         } else if (ph_minimum < ph){
           Serial.print(" PH Kolam = ");
           Serial.println(ph);
           digitalWrite(asam, LOW);
           digitalWrite(basah, HIGH);
           Serial.print(" Status = ");
           Serial.println(" Poma Air Asam Menyala. ");
         }
      

/*
      if (ph_maximum > ph) {
        Serial.print(" PH from cek asam = ");
        Serial.println(ph);
        digitalWrite(basah, LOW);
        digitalWrite(asam, HIGH);
      }
       else if (ph_minimum < ph) {
        Serial.println(ph);
        digitalWrite(asam, LOW);
        digitalWrite(basah, HIGH);
      } else if (ph_minimum == ph || ph_maximum == ph) {
        digitalWrite(basah, HIGH);
        digitalWrite(asam, HIGH);
        Serial.println("Normal");
      }

      */


      parsing = false;
      sData = "";
    }
  }



}

void baca_config_serial() {
  while (komunikasi.available()) {
    char inChar = komunikasi.read();
    //inChar.trim();
    sData += inChar;
    if (inChar == '$') {
      parsing = true;
    }
    if (parsing) {
      int q = 0;
      for (int i = 0; i < sData.length(); i++) {
        if (sData[i] == '#') {
          q++;
          data[q] = "";
        } else {
          data[q] += sData[i];
        }
      }
      Serial.println("Data Masuk :" + sData);
      Serial.println("PH Minimum :" + data[1]);
      Serial.println("PH Maximum :" + data[2]);
      parsing = false;
      sData = "";
    }
  }

}

void store_ph() {

  //String nilai_ph = String(kalibrasi2());
  komunikasi.print(kalibrasi2());
  
}


void setup() {
  Serial.begin(9600);
  komunikasi.begin(9500);
  pinMode(asam, OUTPUT);
  pinMode(basah, OUTPUT);
  digitalWrite(basah, HIGH);
  digitalWrite(asam, HIGH);
}

void loop() {
  // kalibrasi();
  //baca_config_serial();
  cek();
  store_ph();


  delay(1500);
}
