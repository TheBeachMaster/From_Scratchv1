

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
#define LED_Amber 2
#define LED_Green 5

/////////////
//#define Relay A3
//#define LED_Red 3
//#define LED_Amber 2
//#define LED_Green 6

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
void grantAccess();
void denyAccess();
void pinSetup();
void deleteOrdinary();
void readMaster_SD();
void readOrdinary_SD();
void tagSwiped();

File myFile;
byte readCard[7]; // Array to store UID of a Single Tag temporarily
char SD_buffer[10];
char card_buffer[10];
char write_buffer[10];
char _match = 0; // Flag for checking if there was a match
bool _isMaster; // Set to true if file is found in Master.txt
bool _isOrdinary; //Set to true if file is found in Ordinary.txt
char _tagSwiped=0;
char _progswipe = 0;

char DataBucket[] = {}; // Temporarily Stores Scanned UID into a single variable
char ch;

void setup() {
  CommsInit(); // Initializes Serial and SPI communication Protocals
  LCDInit();
  RFIDInit();
  CreateFile(); // Creates files in SD Card
}
void loop() {
   CheckMaster();
// programMode();
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
  if ( !rfid.PICC_IsNewCardPresent()) {
    return 0; //If a new Access Card is placed to RFID reader continue
  }
  if ( !rfid.PICC_ReadCardSerial()) {
    return 0; //Since a Tag is placed, get Serial and continue
  }
  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)
  Serial.println(F("Scanned Access Tag Unique ID:"));

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];

    // Operation on lower byte to enable ID in a nibble rather than a byte
    byte lowerByte = (readCard[i] & 0x0F);
    int lowerInt = (int) lowerByte; //converting byte into int
    char lowerChar[1];
    itoa(lowerInt, lowerChar, 16);
    byte upperByte = (readCard[i] >> 4);
    int upperInt = (int) upperByte;

    // Operation on upper byte to enable ID in a nibble rather than a byte
    char upperChar[1];
    itoa(upperInt, upperChar, 16);
    int j = i * 2;
    int k = j + 1;

    // Storing UID in char array (card_buffer)
    card_buffer[k] = lowerChar[0];
    card_buffer[j] = upperChar[0];
  }
  Serial.print("card buffer");
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
    for (int i = 0; i < 4; i++) {

      byte lowerByte = (readCard[i] & 0x0F);
      int lowerInt = (int) lowerByte;
      char lowerChar[1];
      itoa(lowerInt, lowerChar, 16);

      byte upperByte = (readCard[i] >> 4);
      int upperInt = (int) upperByte;
      char upperChar[1];
      itoa(upperInt, upperChar, 16);

      int j = i * 2;
      int k = j + 1;
      write_buffer[k] = lowerChar[0];
      write_buffer[j] = upperChar[0];
    }

    Serial.println(write_buffer);
    myFile.print(write_buffer);

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
  delay(200);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MARAFIQUE LIFT");
  lcd.setCursor(-1, 1);
  lcd.print("Please Place Tag");
  lcd.setCursor(-3, 2);
  lcd.print("to call Lift...");
  lcd.setCursor(-2, 3);


}
void RFID_disable() {
  pinMode(RFID_SS, INPUT);
  digitalWrite(RFID_SS, HIGH);
}
void CheckMaster() { // changed from boolean
  SD_Disable();
 // RFIDInit();
  //digitalWrite(RFID_SS, LOW);
   rfid.PCD_Init();
  if ( !rfid.PICC_IsNewCardPresent()) {
      return 0; //If a new Access Card is placed to RFID reader continue
  }
  if ( !rfid.PICC_ReadCardSerial()) {
      return 0; //Since a Tag is placed, get Serial and continue
  }
  RFID_disable();
  pinMode (SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];

    // Operation on lower byte to enable ID in a nibble rather than a byte
    byte lowerByte = (readCard[i] & 0x0F);
    int lowerInt = (int) lowerByte; //converting byte into int
    char lowerChar[1];
    itoa(lowerInt, lowerChar, 16);
    byte upperByte = (readCard[i] >> 4);
    int upperInt = (int) upperByte;

    // Operation on upper byte to enable ID in a nibble rather than a byte
    char upperChar[1];
    itoa(upperInt, upperChar, 16);
    int j = i * 2;
    int k = j + 1;

    // Storing UID in char array (card_buffer)
    card_buffer[k] = lowerChar[0];
    card_buffer[j] = upperChar[0];
  }
  Serial.print("card buffer:");
  Serial.println(card_buffer);
  rfid.PICC_HaltA(); // Stop reading
  Serial.println("initialization done.");
  readMaster_SD();
}
void readMaster_SD() {
  if (!myFile) {
  SD_Enable();
  //RFID_disable();
  Serial.println("opening file from SD");
  myFile = SD.open("Master.txt");}
  if (myFile) {
    Serial.println("Master.txt:");

    // read from the file until there's nothing else in it:
    uint8_t i = 0;
    while (myFile.available()) {
      char ch = myFile.read();
      Serial.print(ch);
          if (ch == ',') {
              i=0;
               if (strstr(SD_buffer, card_buffer) > 0) {
                Serial.println("");
                Serial.println("Match Found in Master");
                Serial.println("********************************************************");
                _match=1;
                _isMaster=1;
                Serial.println("Entering Program Mode");
                programMode();
               }
               else _match=0;
               } else {
                SD_buffer[i]=ch;
                i++;
               
      }
    }
    if (!_match) {
      Serial.println("");
      Serial.println("Match Not Found");
      Serial.println("********************************************************");
      readOrdinary_SD();
      return;
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Master.txt");
  }
 // SD_Disable();
}
void programMode(){ // Allows for deletion and addition of new tags
   Serial.println(_isMaster);
  if (_isMaster) {
    Serial.println("Entered Prog Mode loop");
  }
  Serial.println("Exited Prog Loop");
}
void tagSwiped(){
  if (rfid.PICC_IsNewCardPresent()) {
       _tagSwiped=1;
       //If a new Access Card is placed to RFID reader continue
  }
}
void CheckOrdinary() { // changed from boolean
  SD_Disable();
  RFIDInit();
  if ( !rfid.PICC_IsNewCardPresent()) {
      return 0; //If a new Access Card is placed to RFID reader continue
  }
  if ( !rfid.PICC_ReadCardSerial()) {
      return 0; //Since a Tag is placed, get Serial and continue
  }
  //RFID_disable();
  SPI.begin();
  pinMode (SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for (int i = 0; i < 4; i++) {
    readCard[i] = rfid.uid.uidByte[i];

    // Operation on lower byte to enable ID in a nibble rather than a byte
    byte lowerByte = (readCard[i] & 0x0F);
    int lowerInt = (int) lowerByte; //converting byte into int
    char lowerChar[1];
    itoa(lowerInt, lowerChar, 16);
    byte upperByte = (readCard[i] >> 4);
    int upperInt = (int) upperByte;

    // Operation on upper byte to enable ID in a nibble rather than a byte
    char upperChar[1];
    itoa(upperInt, upperChar, 16);
    int j = i * 2;
    int k = j + 1;

    // Storing UID in char array (card_buffer)
    card_buffer[k] = lowerChar[0];
    card_buffer[j] = upperChar[0];
  }
  Serial.println("card buffer");
  Serial.println(card_buffer);
  rfid.PICC_HaltA(); // Stop reading
  if (!SD.begin(SD_CS)) { // Necessary to debug printing junk
  }
  Serial.println("initialization done.");
  SD_Enable();
  readOrdinary_SD();
  Serial.println("///////////////////////////////////////////////////");
 
}
void SD_Disable() {
  digitalWrite(SD_POWER, HIGH);
  digitalWrite(SD_CS, HIGH);
}
void SD_Enable() {
  if (!SD.begin(SD_CS)) {
//    Serial.println("initialization failed!");
//    return;
      digitalWrite(SD_POWER, LOW);
      digitalWrite(SD_CS, LOW);
  }
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
void pinSetup() {
  pinMode(Relay, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Amber, OUTPUT);
  pinMode(LED_Green, OUTPUT);
}
void deleteOrdinary() { // Deletes tag in ordinary.txt file

}
void readOrdinary_SD() {
  myFile.close();
   if (!myFile) {
  SD_Enable();
  //RFID_disable();
  Serial.println("opening file from SD");
  myFile = SD.open("Ordinary.txt");}
  if (myFile) {
    Serial.println("Ordinary.txt:");

    // read from the file until there's nothing else in it:
    uint8_t i = 0;
    while (myFile.available()) {
      char ch = myFile.read();
      Serial.print(ch);
          if (ch == ',') {
              i=0;
               if (strstr(SD_buffer, card_buffer) > 0) {
                Serial.println("");
                Serial.println("Match Found in Ordinary File");
                Serial.println("********************************************************");
                //_match=1;
                _isMaster=false;
                 Serial.println("Grant Access");
                 grantAccess();
                 return;
               }
               else _match=0;
               } else {
                SD_buffer[i]=ch;
                i++;
               
      }
    }
    if (!_match) {
      Serial.println("");
      Serial.println("Match Not Found");
      Serial.println("********************************************************");
      Serial.println("Unknown Tag");
      denyAccess();
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Master.txt");
  }
}
