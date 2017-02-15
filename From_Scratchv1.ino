

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
LiquidCrystal_I2C lcd(0x27, 16, 4);

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

void setup() {

  CommsInit(); // Initializes Serial and SPI communication Protocals
  LCDInit(); // Initializes LCD and prints out initial message
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
  Serial.println("");
<<<<<<< HEAD
  cardType();
//  if (IsMaster) {
//    WriteMaster();
//  } else {
//    WriteOrdinary();
//  }
=======
  //Invoke CardType Func to check the Access CardType
  cardType();
  if (IsMaster) {
    WriteMaster();
  } else if(IsOrdinary) {
    //TO DO Disambiguate
    WriteOrdinary();
  }else{
    //Reject
  }
>>>>>>> 4f5642bef0a828de8f7759214cd894d9905dc94d
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
  
  myFile = SD.open("Master.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to Master.txt...");

    // myFile.print(readCard[0]);
    // myFile.print(readCard[1]);
    // myFile.print(readCard[2]);
    // myFile.print(readCard[3]);

      unsigned long UID_unsigned;
      UID_unsigned =  mfrc522.uid.uidByte[0] << 24;
      UID_unsigned += mfrc522.uid.uidByte[1] << 16;
      UID_unsigned += mfrc522.uid.uidByte[2] <<  8;
      UID_unsigned += mfrc522.uid.uidByte[3];

      // Serial.println();
      // Serial.println("UID Unsigned int"); 
      // Serial.println(UID_unsigned);

      String UID_string =  (String)UID_unsigned;
      long UID_LONG=(long)UID_unsigned;

      // Serial.println("UID Long :");
      // Serial.println(UID_LONG);

      // Serial.println("UID String :");
      // Serial.println(UID_string);

      myFile.print(UID_string);

    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Master.txt");
  }
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


      unsigned long UID_unsigned;
      UID_unsigned =  mfrc522.uid.uidByte[0] << 24;
      UID_unsigned += mfrc522.uid.uidByte[1] << 16;
      UID_unsigned += mfrc522.uid.uidByte[2] <<  8;
      UID_unsigned += mfrc522.uid.uidByte[3];

        // Serial.println();
        // Serial.println("UID Unsigned int"); 
        // Serial.println(UID_unsigned);

      String UID_string =  (String)UID_unsigned;
      long UID_LONG=(long)UID_unsigned;

      // Serial.println("UID Long :");
      // Serial.println(UID_LONG);

      // Serial.println("UID String :");
      // Serial.println(UID_string);    

    // myFile.print(readCard[0]);
    // myFile.print(readCard[1]);
    // myFile.print(readCard[2]);
    // myFile.print(readCard[3]);

    myFile.print(UID_string);//Prints the UID Card in String Format

    myFile.print(",");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Ordinary.txt");
  }
}
void cardType(){  
  //Replaced the Booloean Func with void Func
  //Variables declared are IsMaster and IsOrdinary

//Initialize SD Card 
      if (!SD.begin(SD_CS)) 
      {
          SD.begin(SD_CS);
          Serial.println("Initializing SD Card...");
      }	
      //	else{
        //Code Logic ---If the SD Card is not initialized,Exit Code with an Error,else set up files for reading
        // Changed logit to --If the SD Card is not ready, instead of exiting. Ran SD initializing code and continue with the rest of the program.
        //Local Variables only
       File masterFile = SD.open("Master.txt");
       File ordinaryFile = SD.open("Ordinary.txt");
       
       int _UIDAvailable = 0 ;//This variable helps us in identifying how many times a  particular UID item appears on the master file
      
       //Open Master File for Reading
       if(masterFile.available()){
         String _masterRead = masterFile.read(); //We're assuming we're reading Strings from the Master File 

         for(int i = 0 ; i <_masterRead.length();i++){
           if(_masterRead.substring(i ,i+1) == ","){
              _UIDAvailable = _masterRead.lastIndexOf("RFUID");
           }
         }
         if(_UIDAvailable == 1){
           IsMaster = true;
         }
       }else if(ordinaryFile.available()){
        String _ordinaryRead = ordinaryFile.read();

        for(int i = 0;i<_ordinaryRead.length();i++){
          if(_ordinaryRead.substring(i,i+1) == ","){ //Use delimiter to find UID Text
            _UIDAvailable =_ordinaryRead.lastIndexOf("RFID");
          }
        }
        if(_UIDAvailable == 1){
          IsOrdinary = true;
        }
       }

      }    
//}
void LCDInit () {
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
  // Print a message to the LCD.
  lcd.setCursor(1, 0);
  lcd.print("Marafique Lift");
  lcd.setCursor(0, 1);
  lcd.print("System  Booting!");
  lcd.setCursor(-3, 2);
  lcd.print("Please Wait...");
  lcd.setCursor(-2, 3);
  lcd.print("KASP3R TECH!");
}
void MainLoop() {
  // RFID scans Tag
  // UID is passed to IsMaster Function that determines whether to invoke program mode( Function Program Mode) or access mode (Function Access Mode)
  // In Program Mode; Grep Function checks if UID is available in Master.txt. If so, any tag scanned is stored in Ordinary.txt using WriteOrdinary function.
  // If Master is scanned again, program exits program mode into Access Mode(Default Mode)
}

<<<<<<< HEAD
=======
void Authorize(){
  //Actuate Relay
}

<<<<<<< HEAD

////We've been able to Create and Write to Files but can not tell the program to jump into
////Code that will allow it to check for UIDs
////Let me know how this code will be structured
=======
>>>>>>> 205ce5d0873167080d2e386efb95cdc19fbe1b36
>>>>>>> 4f5642bef0a828de8f7759214cd894d9905dc94d
