

/*
  SD card basic file example
*/
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


#define SD_CS 4
#define RFID_SS 10
#define RFID_RST 9
#define SD_POWER 7

MFRC522 rfid(RFID_SS, RFID_RST);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void CommsInit();
void LCDInit();
void RFIDInit();
void CreateFile();
void ReadUID();
void WriteMaster(); // Writes UID to Master File in SD card
void WriteOrdinary(); // Writes UID to Ordinary File in SD card
boolean IsMaster =  false; // Returns true if Scanned Tag's UID is found in Master File
boolean IsOrdinary = false; //Set to true if file is not from Master file

File myFile;
byte readCard[4]; // Array to store UID of a Single Tag temporarily

unsigned long DataBucket; // Temporarily Stores Scanned UID into a single variable
String Databoss;


void setup() {

  CommsInit(); // Initializes Serial and SPI communication Protocals
  LCDInit();
  CreateFile(); // Creates files in SD Card

}

void loop() {
  // nothing happens after setup finishes.
  ReadUID();
  //CheckMatch();
  
}
void CreateFile() {
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");
  lcd.print("SD card init...");
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
  rfid.PCD_Init();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
}
void CommsInit() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
void ReadUID() {
  //Stopping SD Card in readiness for RFID initialization
  RFIDInit();
  // Getting ready for Reading Tags/Cards
  if ( ! rfid.PICC_IsNewCardPresent()) { //If a new Access Card is placed to RFID reader continue
    return 0;
  }
  if ( ! rfid.PICC_ReadCardSerial()) {   //Since a Tag is placed, get Serial and continue
    return 0;
  }
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];
    Serial.print(readCard[i], HEX);

  }
  
      DataBucket = rfid.uid.uidByte[0] ;
      DataBucket += rfid.uid.uidByte[1] ;
      DataBucket += rfid.uid.uidByte[2] ;
      DataBucket += rfid.uid.uidByte[3];
      Serial.println("");
      Serial.println("******************************");
 CheckMatch();
      Serial.println("******************************");
      rfid.PICC_HaltA(); // Stop reading
  return 1;
}
void WriteMaster() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("initialization done.");
  }
  myFile = SD.open("Master.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Master.txt...");

    //     myFile.print(readCard[0]);
    //     myFile.print(readCard[1]);
    //     myFile.print(readCard[2]);
    //     myFile.print(readCard[3]);

    DataBucket = rfid.uid.uidByte[0] << 24;
    DataBucket += rfid.uid.uidByte[1] << 16;
    DataBucket += rfid.uid.uidByte[2] << 8;
    DataBucket += rfid.uid.uidByte[3];

    myFile.print(DataBucket);
    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Master.txt");
  }
}

void WriteOrdinary() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("initialization done.");
  }
  myFile = SD.open("Ordinary.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Ordinary.txt...");

    /*
      We can use thios code to store the variable we want to write to SD Card

      This will generate a String which will be dumped in to the Text file and later on used
      to perform a search query
    */


    // myFile.print(readCard[0]);
    // myFile.print(readCard[1]);
    // myFile.print(readCard[2]);
    // myFile.print(readCard[3]);

    DataBucket = rfid.uid.uidByte[0] << 24;
    DataBucket += rfid.uid.uidByte[1] << 16;
    DataBucket += rfid.uid.uidByte[2] << 8;
    DataBucket += rfid.uid.uidByte[3];

    myFile.print(DataBucket);
    myFile.print(",");

    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Ordinary.txt");
  }
}
void LCDInit () {
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

  lcd.setCursor(1, 0);
  lcd.print("Marafique Lift");
  lcd.setCursor(-1, 1);
  lcd.print("System  Booting!");
  lcd.setCursor(-3, 2);
  lcd.print("Please Wait...");
  lcd.setCursor(-2, 3);
  lcd.print("KASP3R TECH!");
}
void Authorize() {
    //Actuate Relay
  }
void CheckMatch() { // changed from boolean

  pinMode (7, OUTPUT);
  digitalWrite(7, LOW);
  myFile = SD.open("Master.txt");

  if (!myFile) {
    Serial.print("The text file cannot be opened");
    while(1);
  }
 
  while (myFile.available()) {
    Databoss = myFile.readStringUntil(',');
            if (Databoss == *(unsigned long*)DataBucket) { //if (buffer == *&readfile) {
            Serial.println("That's a Master Tag");
            break;
            }
            else {
            myFile.close();
            Serial.println("No Master Tag");
            break;
            }
  }

  myFile.close();
}

void GlobalCheck(){
  /*
  This Code Checks for UIDs in both the Master and Ordinary Files

  /////////////////////Variables///////////

  The Variables being used are:
  1. IsMaster
  2. IsOrdinary
  3. myFile //Not Used
  4. DataBucket
  5. Databoss

  ///////Usage////////////////
  This code can be used immediately after reading tags and replace the CheckMatch() Func above
  

  */

  
  pinMode (7, OUTPUT);
  digitalWrite(7, LOW);

  if(!SD.exists("Master.txt") && !SD.exists("Ordinary.txt")){
    Serial.println("We encountered a problem setting data storage");

  }

  if(SD.open("Master.txt",FILE_READ)){
        Databoss = SD.open("Master.txt").readStringUntil(',');
            if (Databoss == *(unsigned long*)DataBucket) { //if (buffer == *&readfile) {
            Serial.println("That's a Master Tag");
            IsMaster = true;
            break;
            }
            else {
              
            Serial.println("No Master Tag");
            SD.close("Master.txt");
            break;
            }
            
      SD.close("Master.txt");
  
  }else if(SD.open("Ordinary.txt",FILE_READ)){
        Databoss = SD.open("Oridary.txt").readStringUntil(',');
            if (Databoss == *(unsigned long*)DataBucket) { //if (buffer == *&readfile) {
            Serial.println("That's an Ordinary Tag");
            IsOrdinary = true;
            break;
            }
            else {
              
            Serial.println("No Ordinary Tag");
            SD.close("Ordinary.txt");
            break;
            }
            
      SD.close("Master.txt");



  }else{
    Serial.println("We had a problem identifying your tag");
  }
}

