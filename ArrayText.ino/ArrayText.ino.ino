void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){
    return;
    }

    GrepMaster();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void GrepMaster(){
  uint16_t RFArray[]={12454};
  uint16_t SDArray[]={1,3,54,6,54,2,12454};
  uint16_t SDval;
  uint16_t RFval;

  

    RFval = RFArray[0];
    for (uint8_t i=0; i<sizeof(SDArray); i++){
    SDval = SDArray[i];
    if (RFval == SDval) {
//      Serial.println(" Same");
//      Serial.println(RFval);
//      Serial.println(SDval);
     FuncA();
      return 0;
    }
    else 
    {
     ;
    }
    }
    
   }

   void FuncA(){
    Serial.println("Another Function");
    }

