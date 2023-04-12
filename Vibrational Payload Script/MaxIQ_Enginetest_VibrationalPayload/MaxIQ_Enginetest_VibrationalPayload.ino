/*
The following script is for interfacing the ESP-32 Extended Core, the LIS2DH12 accelerometer, the Goertek SPL06-001 barometer,
and the Sensirion SHT30-DIS temperature and humidty sensors.

This script was developed on behalf of MaxIQ Space by Edward Tang from Montgomery High School, Skillman NJ, USA.
Any questions or concerns about the script should be directed to cody@maxiq.space with a cc to daniel@maxiq.space
All troubleshooting should be completed at the local level first, if the issue is not localized and is believed to 
be due to a global issue with the script itself, please reach out and let us know.

All rights reserved.

*/

#include <SPL06-007.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "FS.h"

#include "SparkFun_LIS2DH12.h" 
SPARKFUN_LIS2DH12 accel;    

#define I2C_SDA 26
#define I2C_SCL 27

#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5

double local_pressure = 1024; // Look up local sea level pressure on google // Local pressure from airport website 8/22 of Brunswick, Maine
bool Accerror = false;
bool Barrerror = false;
bool SDerror = false;

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}


void setup() {
  Serial.begin(115200);
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();

  SPIClass spi = SPIClass(HSPI);
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  if (!SD.begin(SD_CS, spi,80000000)) {
    Serial.println(F("Card Mount Failed"));
    SDerror = true;
    return;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println(F("No SD card attached"));
      SDerror = true;
      return;
  }

  if (SD.exists("/data.txt")) {
    SD.remove(F("/data.txt"));
  }
  

  writeFile(SD, "/data.txt", "time(ms),accelX(g),accelY(g),accelZ(g),temp(C),press(mb),alt(m)\n");

  SPL_init(0x77);

  if (accel.begin() == false)
  {
    Serial.println(F("Accelerometer not detected"));
    Accerror = true;
    return;
  }

  accel.setScale(LIS2DH12_16g);
  accel.setMode(LIS2DH12_HR_12bit);
  accel.setDataRate(LIS2DH12_ODR_400Hz);

  while (true){
    float accelX = 0;
    float accelY = 0;
    float accelZ = 0;
    if(Accerror){
        Serial.println(F("accel error"));
        return;
    }else if(SDerror){
        Serial.println(F("sd error"));
        return;
    }else if(Barrerror){
        Serial.println(F("barr error"));
        return;
    }

    long time =  millis();
  //accelerometer
    if (accel.available())
      {
        accelX = accel.getX()*0.001;
        accelY = accel.getY()*0.001;
        accelZ = accel.getZ()*0.001;

      }


    double alt = get_altitude(get_pressure(),local_pressure);
    double press = get_pressure();
    double temp = get_temp_c();

    String data = "";

    data.concat(String(time));
    data.concat(",");
    data.concat(String(accelX,2));
    data.concat(",");
    data.concat(String(accelY,2));
    data.concat(",");
    data.concat(String(accelZ,2));
    data.concat(",");
    data.concat(String(temp,2));
    data.concat(",");
    data.concat(String(press,2));
    data.concat(",");
    data.concat(String(alt,3));
    data.concat("\n");

    //Serial.print(data);

    appendFile(SD, "/data.txt", data.c_str());
  }

}

void loop() {

//   float accelX = 0;
//   float accelY = 0;
//   float accelZ = 0;
//   if(Accerror){
//       Serial.println(F("accel error"));
//       return;
//   }else if(SDerror){
//       Serial.println(F("sd error"));
//       return;
//   }else if(Barrerror){
//       Serial.println(F("barr error"));
//       return;
//   }

//   long time =  millis();
// //accelerometer
//   if (accel.available())
//     {
//       accelX = accel.getX()*0.001;
//       accelY = accel.getY()*0.001;
//       accelZ = accel.getZ()*0.001;

//     }


//   double alt = get_altitude(get_pressure(),local_pressure);
//   double press = get_pressure();
//   double temp = get_temp_c();

//   String data = "";

//   data.concat(String(time));
//   data.concat(",");
//   data.concat(String(accelX,2));
//   data.concat(",");
//   data.concat(String(accelY,2));
//   data.concat(",");
//   data.concat(String(accelZ,2));
//   data.concat(",");
//   data.concat(String(temp,2));
//   data.concat(",");
//   data.concat(String(press,2));
//   data.concat(",");
//   data.concat(String(alt,3));
//   data.concat("\n");

//   Serial.print(data);

//   appendFile(SD, "/data.txt", data.c_str());

}
