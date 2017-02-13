/*
  SD card basic file example
*/
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>

#define SD_CS 4
#define RFID_SS 10
#define RFID_RST 9
#define SD_POWER 7

MFRC522 rfid(RFID_SS, RFID_RST);

void CommsInit();
void RFIDInit(); 
void CreateFile();
void ReadUID();
void WriteMaster(); // Writes UID to Master File in SD card
void WriteOrdinary(); // Writes UID to Ordinary File in SD card
boolean IsMaster(); // Returns true if Scanned Tag's UID is found in Master File

File myFile;
byte readCard[4]; // Array to store UID of a Single Tag temporarily

void setup() {
  
  CommsInit(); // Initializes Serial and SPI communication Protocals
  CreateFile(); // Creates files in SD Card


}

void loop() {
  // nothing happens after setup finishes.
  ReadUID();
}
void CreateFile() {
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");
      if (!SD.begin(SD_CS)) {
                  Serial.println("initialization failed!");
                  return;
                            }
        Serial.println("initialization done.");

      if (!SD.exists("Master.txt")) {
                  Serial.println("Creating Master.txt...");
                  myFile = SD.open("Master.txt", FILE_WRITE);
                  myFile.close();
                  delay(200);
                  Serial.println("Master.txt File Created.");
              }
      if (!SD.exists("Ordinary.txt")) {
                  Serial.println("Creating Ordinary.txt...");
                  myFile = SD.open("Ordinary.txt", FILE_WRITE);
                  myFile.close();
                  delay(200);
                  Serial.println("Ordinary.txt File Created.");
              }
  }
 
void RFIDInit() {//Creates an Instance of RFID Object called rfid and initializes it
  //Starting RFID and stopping SD
  digitalWrite(SD_CS, HIGH);
  digitalWrite(SD_POWER, HIGH);
  pinMode(RFID_SS, OUTPUT);
  digitalWrite(RFID_SS, LOW);
  MFRC522 rfid(RFID_SS, RFID_RST); 
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
}
void CommsInit() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.begin();
  while (!Serial) {
    
  SysCall :: yield();
     // wait for serial port to connect. Needed for native USB port only
  }
}
void ReadUID() {
  //Stopping SD Card in readiness for RFID initialization
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode (SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, HIGH);
  // Getting ready for Reading Tags/Cards
  if ( ! rfid.PICC_IsNewCardPresent()) { //If a new Access Card is placed to RFID reader continue
    return 0;
  }
  if ( ! rfid.PICC_ReadCardSerial()) {   //Since a Tag is placed, get Serial and continue
    return 0;
  }
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = rfid.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  rfid.PICC_HaltA(); // Stop reading
  return 1; 
}
void WriteMaster() {
       ReadUID();
       myFile = SD.open("Master.txt", FILE_WRITE);
       for (uint8_t i=0; i<3; i++) {
        myFile.print(readCard[i]);
       }
       myFile.print(",");
       myFile.close();
       Serial.println("Writing to Master File done."); 
}
void WriteOrdinary() {
      ReadUID();
       myFile = SD.open("Ordinary.txt", FILE_WRITE);
       for (uint8_t i=0; i<3; i++) {
        myFile.print(readCard[i]);
       }
       myFile.print(",");
       myFile.close();
       Serial.println("Writing to Ordinary File done."); 
}
void ReadSDFiles(){
   if (!SD.begin(SD_CS_PIN)) {
    Serial.println("An Error Ocuured while Initializing SD Card");
    return;
  }

  
}
