

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
#define Relay 2
#define LED_Red 3
#define LED_Amber 5
#define LED_Green 6


MFRC522 rfid(RFID_SS, RFID_RST);
LiquidCrystal_I2C lcd(0x27, 16, 4);

void CommsInit();
void LCDInit();
void RFIDInit();
void CreateFile();
void ReadUID();
void WriteMaster(); // Writes UID to Master File in SD card
void WriteOrdinary(); // Writes UID to Ordinary File in SD card
void CheckOrdinary();
void CheckMaster();
void SD_Enable();
void SD_Disable();
void programMode();
void accessMode();
void grantAccess();
void denyAccess();
void pinSetup();
void deleteOrdinary();

File myFile;
byte readCard[7]; // Array to store UID of a Single Tag temporarily
char SD_buffer[10];
char card_buffer[10];
char write_buffer[10];
uint8_t _match=0; // Flag for checking if there was a match
boolean _isMaster =  false; // Set to true if file is found in Master.txt
boolean _isOrdinary = false; //Set to true if file is found in Ordinary.txt

char DataBucket[] ={}; // Temporarily Stores Scanned UID into a single variable
char ch="";

void setup() {
  CommsInit(); // Initializes Serial and SPI communication Protocals
  RFIDInit();
  LCDInit();
  CreateFile(); // Creates files in SD Card 
}
void loop() {
  //ReadUID();
  CheckMaster();
  //____________main loop_________________________
//  if (_isMaster) {
//    Serial.println ("Master Tag Swiped");
//    programMode();
//  }
//  else if (!_isMaster){
//    Serial.println ("Ordinary Tag Swiped");
//    accessMode ();
//  } else {
//    Serial.println ("Unknown Tag!");
//  }
//______________________________________________
}
void CreateFile() {
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD card...");
//  lcd.clear();
//  lcd.setCursor(-2,1);
//  lcd.print("SD card init...");
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
void RFIDInit() {
  //Creates an Instance of RFID Object called rfid and initializes it
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
  RFIDInit();
  SD_Disable();
  // Getting ready for Reading Tags/Cards
  if ( !rfid.PICC_IsNewCardPresent()) {return 0;} //If a new Access Card is placed to RFID reader continue
  if ( !rfid.PICC_ReadCardSerial()) {return 0;}   //Since a Tag is placed, get Serial and continue
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];

    // Operation on lower byte to enable ID in a nibble rather than a byte
    byte lowerByte = (readCard[i]&0x0F);
    int lowerInt = (int) lowerByte; //converting byte into int
    char lowerChar[1];
    itoa(lowerInt,lowerChar,16);
    byte upperByte = (readCard[i] >> 4);
    int upperInt = (int) upperByte;
    
    // Operation on upper byte to enable ID in a nibble rather than a byte
    char upperChar[1];
    itoa(upperInt,upperChar,16);
    int j=i*2;
    int k=j+1;
    
    // Storing UID in char array (card_buffer)
    card_buffer[k] = lowerChar[0];
    card_buffer[j] = upperChar[0];
  }
    Serial.println(card_buffer);
    rfid.PICC_HaltA(); // Stop reading
    return 1;
}
void WriteMaster() {
  SD_Enable();
  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("initialization done.");
  }
  myFile = SD.open("Master.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Master.txt...");

//    myFile.print(readCard[0], HEX);
//    myFile.print(readCard[1], HEX);
//    myFile.print(readCard[2], HEX);
//    myFile.print(readCard[3], HEX);

    ///////////////////////////////////////////
    for (int i = 0; i < 4; i++) {
    
      byte lowerByte = (readCard[i] & 0x0F);
      int lowerInt = (int) lowerByte;
      char lowerChar[1];
      itoa(lowerInt,lowerChar,16);
  
      byte upperByte = (readCard[i] >> 4);
      int upperInt = (int) upperByte;
      char upperChar[1];
      itoa(upperInt,upperChar,16);
  
      int j=i*2;
      int k=j+1;
      write_buffer[k] = lowerChar[0];
      write_buffer[j] = upperChar[0];    
    }

    Serial.println(write_buffer);
    myFile.print(write_buffer);
    

    //////////////////////////////////////////

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


    myFile.print(readCard[0], HEX);
    myFile.print(readCard[1], HEX);
    myFile.print(readCard[2], HEX);
    myFile.print(readCard[3], HEX);

    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Ordinary.txt");
  }
}
void LCDInit() {
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MARAFIQUE LIFT");
  lcd.setCursor(-1, 1);
  lcd.print("System Bootiing!");
  lcd.setCursor(-3, 2);
  lcd.print("Please wait...");
  lcd.setCursor(-2, 3);
  lcd.print("KASP3R TECH!");
  delay(3000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MARAFIQUE LIFT");
  lcd.setCursor(-1, 1);
  lcd.print("Please Place Tag");
  lcd.setCursor(-3, 2);
  lcd.print("to call Lift...");
  lcd.setCursor(-2, 3);
  
  
}
void CheckMaster() { // changed from boolean
  RFIDInit();
  SD_Disable();
  // Getting ready for Reading Tags/Cards
  if ( !rfid.PICC_IsNewCardPresent()) {return 0;} //If a new Access Card is placed to RFID reader continue
  if ( !rfid.PICC_ReadCardSerial()) {return 0;}   //Since a Tag is placed, get Serial and continue
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];

    // Operation on lower byte to enable ID in a nibble rather than a byte
    byte lowerByte = (readCard[i]&0x0F);
    int lowerInt = (int) lowerByte; //converting byte into int
    char lowerChar[1];
    itoa(lowerInt,lowerChar,16);
    byte upperByte = (readCard[i] >> 4);
    int upperInt = (int) upperByte;
    
    // Operation on upper byte to enable ID in a nibble rather than a byte
    char upperChar[1];
    itoa(upperInt,upperChar,16);
    int j=i*2;
    int k=j+1;
    
    // Storing UID in char array (card_buffer)
    card_buffer[k] = lowerChar[0];
    card_buffer[j] = upperChar[0];
  }
    Serial.println(card_buffer);
    rfid.PICC_HaltA(); // Stop reading
   // return 1;
      
//    digitalWrite(RFID_SS, HIGH);
    SD_Enable();
      myFile = SD.open("Master.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    uint8_t i=0;
    while (myFile.available()) {
      char ch = myFile.read();
      Serial.print(ch);
      if (ch == ','){
        i=0;
        if (strstr(SD_buffer, card_buffer)>0){
          Serial.println("Match Found");
          _match = 1;
          break;
        } 
      } else {
        SD_buffer[i] = ch;
        i++;
      }
    }
    if (!_match) {
      Serial.println("Get a lyf...!!!!!");
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  /*if (myFile) {
    Serial.println("*****************");
    Serial.println("Master.txt:");
    //Serial.println(myFile.read());
    // read from the file until there's nothing else in it:
    uint8_t i=0;
    while (myFile.available()) {
      ch = myFile.read();
      Serial.write(ch);
      if (ch == ','){
        i=0;
        if (strstr(SD_buffer, card_buffer)>0){
          Serial.println("Match Found");
          _match = 1;
          break;
        } 
      } else {
        SD_buffer[i] = ch;
        i++;
      }
    }
    if (!_match) {
      Serial.println("Get a lyf...!!!!!");
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening master.txt");
  }*/
   Serial.print(ch, HEX);
}
void accessMode(){ // Default Mode: constantly checks if the tag is available in Ordinary.txt file in SD
  
}
//void programMode(){ // Allows for deletion and addition of new tags
//  
//     if (_isOrdinary) {
//      // delete from SD
//      // LCD: feedback tag has been removed
//     } else {
//      // add tag in SD
//      // LCD: feedback tag has been added then prompt for next tag.
//     }
//}
void CheckOrdinary(){
//  pinMode (7, OUTPUT);
//  digitalWrite(7, LOW);
//           myFile = SD.open("Ordinary.txt");
//           while (myFile.available()&&(IsMaster==false)){
//            boss = myFile.readStringUntil(',');
//             if (boss == *(unsigned long*)DataBucket,HEX) {
//              Serial.println("Ordinary Tag");
//              break;
//             }
//             else {
//              Serial.println("Unknown Tag");
//              break;
//             }
//              myFile.close();
//      }
}
void SD_Disable() { 
  digitalWrite(SD_POWER,HIGH);
  digitalWrite(SD_CS,HIGH);
}
void SD_Enable() {
  
  digitalWrite(SD_POWER,LOW);
  digitalWrite(SD_CS,LOW);
}
void grantAccess () { // Actuates Relay, Displays feedback on LCD and LED to show access has been granted
  digitalWrite(Relay, HIGH);
  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Amber, LOW);
  digitalWrite(LED_Green, HIGH);
}
void denyAccess () { // Actuates Relay, Displays feedback on LCD and LED to show access has been denied
  digitalWrite(Relay, LOW);
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Amber, LOW);
  digitalWrite(LED_Green, LOW);
}
void pinSetup(){
  pinMode(Relay, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Amber, OUTPUT);
  pinMode(LED_Green, OUTPUT);
}
void programMode(){
  digitalWrite(Relay, LOW);
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Amber, HIGH);
  digitalWrite(LED_Green, LOW);
  
  while (_isMaster){
    if (_isMaster/*there was Ordinary match*/) {
      deleteOrdinary();
    } else {
       WriteOrdinary();
    }
  }
  _isMaster == false; // write false to Master flag to return to exit program mode with every deletion or addition of a new tag
}
void deleteOrdinary(){// Deletes tag in or
  
}

