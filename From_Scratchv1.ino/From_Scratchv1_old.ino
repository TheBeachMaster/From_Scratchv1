

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
void WriteMaster(); // Writes UID to Master File in SD card
void WriteOrdinary(); // Writes UID to Ordinary File in SD card
void CheckOrdinary();
int CheckMaster();
void SD_Enable();
void SD_Disable();
void programMode();
void accessMode();
void grantAccess();
void denyAccess();
void pinSetup();
void deleteOrdinary();
int read_SD(char theFile);

File myFile;
byte readCard[7]; // Array to store UID of a Single Tag temporarily
char SD_buffer[10];
char card_buffer[10];
char write_buffer[10];
char master_buffer[10];
uint8_t _match=0; // Flag for checking if there was a match
boolean _isMaster =  false; // Set to true if file is found in Master.txt
boolean _isOrdinary = false; //Set to true if file is found in Ordinary.txt

char DataBucket[] = {}; // Temporarily Stores Scanned UID into a single variable
char ch;
uint8_t programmingModeIsOn=0;

void setup() {
  CommsInit(); // Initializes Serial and SPI communication Protocals
  RFIDInit();
  LCDInit();
  CreateFile(); // Creates files in SD Card
}
void loop() {
  CheckMaster();
  
}
void CreateFile() {
  pinMode(SD_POWER, OUTPUT);
  digitalWrite(SD_POWER, LOW);
  Serial.println("Initializing SD");
  if (!SD.begin(SD_CS)) {
    Serial.println("init failed!");
    return;
  }
  Serial.println("init done.");

  if (!SD.exists("Master.txt")) {
    Serial.println("Creating Master.txt");
    myFile = SD.open("Master.txt", FILE_WRITE);
    myFile.close();
    delay(200);
    Serial.println("Master.txt Created.");
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

    //    myFile.print(readCard[0], HEX);
    //    myFile.print(readCard[1], HEX);
    //    myFile.print(readCard[2], HEX);
    //    myFile.print(readCard[3], HEX);

    ///////////////////////////////////////////
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
   SD_Enable();
  Serial.println("Init SD card...");

  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    Serial.println("init done.");
  }
  myFile = SD.open("Ordinary.txt", FILE_WRITE);
   if ( !rfid.PICC_IsNewCardPresent()) {
    return 0; //If a new Access Card is placed to RFID reader continue
  }
  if ( !rfid.PICC_ReadCardSerial()) {
    return 0; //Since a Tag is placed, get Serial and continue
  }
  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print("Writing to Ordinary.txt...");

//        myFile.print(readCard[0], HEX);
//        myFile.print(readCard[1], HEX);
//        myFile.print(readCard[2], HEX);
//        myFile.print(readCard[3], HEX);

    ///////////////////////////////////////////
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


    //////////////////////////////////////////

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
void RFID_disable() {
  pinMode(RFID_SS, OUTPUT);
  digitalWrite(RFID_SS, HIGH);
}
int CheckMaster() { // changed from boolean
  RFIDInit();
  SD_Disable();
  _match=0;
  if ( !rfid.PICC_IsNewCardPresent()) {
    //Serial.println("No card");
    return 0; //If a new Access Card is placed to RFID reader continue
  }
  if ( !rfid.PICC_ReadCardSerial()) {
   // Serial.println("Not read");
    return 0; //Since a Tag is placed, get Serial and continue
  }
  //RFID_disable();
  SPI.begin();
  pinMode (7, OUTPUT);
  digitalWrite(7, LOW);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Assuming Tags have 4 byte UID, others may have 7 (Reminder)

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
  // return 1;
  //Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
  }
  //Serial.println("initialization done.");
    SD_Enable();
  int cardIsMaster=read_SD("Master.txt");
  if(cardIsMaster) {
    Serial.println("Card Master");
      if(programmingModeIsOn){
          programmingModeIsOn=0;
       //    Serial.println("Program Mode OFF");
           lcd.setCursor(-2,1);
           lcd.print("Program Mode:OFF");
           lcd.setCursor(-4,2);
           lcd.print("Resuming Access ");
           lcd.setCursor(2,3);
           lcd.print("Mode");
           delay(2000);
           lcd.clear();
           lcd.setCursor(1, 0);
           lcd.print("MARAFIQUE LIFT");
           lcd.setCursor(-1, 1);
           lcd.print("Please Place Tag");
           lcd.setCursor(-3, 2);
           lcd.print("to call Lift...");
           lcd.setCursor(-2, 3);
           
      }
      else{
        programmingModeIsOn=1;
        //LCD display programming mode
       //  Serial.println("Program Mode ON");

         lcd.setCursor(-2,1);
         lcd.print("Program Mode:ON ");
         lcd.setCursor(-4,2);
         lcd.print("Please Place Tag.");
      }
    }
   
   // _match=0;
    if (!SD.begin(4)) {
    }
    int cardIsOrd = read_SD("Ordinary.txt");
    if(cardIsOrd){
       Serial.println(cardIsOrd);
       
      if(programmingModeIsOn==1 && cardIsOrd==1){
       //  Serial.println("removing card");
         deleteOrdinary();
         lcd.setCursor(-4,3);
         lcd.print("Key Tag: Deleted.");
         delay(500);
         lcd.setCursor(-4,3);
         lcd.print("                ");
      }
       if (programmingModeIsOn==1 && (cardIsOrd==0)) {
       Serial.println(programmingModeIsOn);
    Serial.println("the point");
         WriteOrdinary();
         lcd.setCursor(-4,3);
         lcd.print("Key Tag: Added.");
         delay(500);
         lcd.setCursor(-4,3);
         lcd.print("                ");
      }
       if(cardIsOrd && programmingModeIsOn==0){
       Serial.println("Card Ord");
       Serial.println("No point");
       //grantAccess();
    }
    }
    else {
      Serial.println("unknown");
      Serial.println(cardIsOrd);
      Serial.println(programmingModeIsOn);
      denyAccess();
    }
   
   
}
int read_SD(char * theFile) {
  Serial.println();
  SD_Enable();
  RFID_disable();
  delay(1000);
  //SD_Enable();
  Serial.println("opening file from SD");
  myFile = SD.open(theFile);
  if (myFile) {
    Serial.println(theFile);

    // read from the file until there's nothing else in it:
    uint8_t i = 0;
    while (myFile.available()) {
      //Serial.println("In file now");
      //delay(200);
      //Serial.write(myFile.read());
      char ch = myFile.read();
      //Serial.print(ch);
      //delay(500);
      if (ch == ',') {
        Serial.print("SD_buffer: ");
        Serial.println(SD_buffer);
        Serial.print("Card_buffer: ");
        Serial.println(card_buffer);
        i = 0;
        if (strstr(SD_buffer, card_buffer) > 0) {
          Serial.println("Match Found");
          _match =1;
          break;
        }
  else { _match=0;}     } else {
        SD_buffer[i] = ch;
        i++;
      }
    }
    //Serial.println();
    //Serial.println(SD_buffer);
    if (!_match) {
      Serial.println("Get a lyf...!!!!!");
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
   // Serial.println("error opening test.txt");
    myFile.close();
  }
  return _match;
}
void SD_Disable() {
  digitalWrite(SD_POWER, HIGH);
  digitalWrite(SD_CS, HIGH);
}
void SD_Enable() {
 digitalWrite(SD_POWER, LOW);
  digitalWrite(SD_CS, LOW);
}
void grantAccess () { // Actuates Relay, Displays feedback on LCD and LED to show access has been granted
  digitalWrite(Relay, HIGH);
  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Amber, LOW);
  digitalWrite(LED_Green, HIGH);

  lcd.init();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MARAFIQUE LIFT");
  lcd.setCursor(-1, 1);
  lcd.print("Access Granted");
  lcd.setCursor(-3, 2);
  lcd.print("Please Wait...");
  lcd.setCursor(-2, 3);
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
void denyAccess () { // Actuates Relay, Displays feedback on LCD and LED to show access has been denied
  digitalWrite(Relay, LOW);
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Amber, LOW);
  digitalWrite(LED_Green, LOW);

  lcd.init();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MARAFIQUE LIFT");
  lcd.setCursor(1, 1);
  lcd.print("Access Denied");
  lcd.setCursor(-4, 2);
  lcd.print("Residents Only..");
  lcd.setCursor(-2, 3);
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
void pinSetup() {
  pinMode(Relay, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Amber, OUTPUT);
  pinMode(LED_Green, OUTPUT);
}
void programMode() {
  digitalWrite(Relay, LOW);
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Amber, HIGH);
  digitalWrite(LED_Green, LOW);

//  /hile (_isMaster) {
//  / if (_isOrdinary/*there was Ordinary match*/) {
//  /   deleteOrdinary();
//  / } else {
//  /   WriteOrdinary();
//  / }
//  /
//  /isMaster == false; // write false to Master flag to return to exit program mode with every deletion or addition of a new tag
}
void deleteOrdinary() { // Deletes tag in ordinary.txt file
 
  if (!SD.begin(SD_CS)) {
    SD.begin(SD_CS);
    //Serial.println("initialization done.");
  } SD_Enable();
 
//  myFile =  SD.remove(card_buffer);
  return;
}

