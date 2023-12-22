/* adapte depuis le code de
    V2.50 - 07/04/2023 - Daniel Desmartins
   Connected to the Relay Port in AgOpenGPS

  Ecriture d'un message  sur 7 section pour feter  noel
   bric bric  20/12/2023
   
>>files  to generate dot  matrix 
https://docs.google.com/spreadsheets/d/1SNG4S7sTqmZtR76YS03U3PU_Yl2-DZqmN_SXQ4Z_i8E/edit?usp=sharing

connect USB  arduino Nano to AOG as machine  on AgOpenGps  (  test  in 5.7.  5.8  ,   normally work in 5.x)
on Arduino Nano connect A0  to GND  to  start ...

 

*/
// merry christmas
//byte message[] = {  0,  0,  127,  2,  4,  2,  127,  0,  127,  73, 73, 65, 0,  126,  25, 41, 78, 0,  126,  25, 41, 78, 0,  3,  4,  120,  4,  3,  0,  0,  0,  0,  62, 65, 65, 34, 0,  127,  8,  8,  127,  0,  126,  25, 41, 78, 0,  0,  65, 127,  65, 0,  46, 73, 73, 58, 0,  1,  1,  127,  1,  1,  0,  127,  2,  4,  2,  127,  0,  0,  126,  9,  9,  126,  0,  46, 73, 73, 58, 0,  0,  0,  0,  0,  0,  20, 30, 127,  30, 20, 0};

// merry christmas dark start
byte message[] = {0,  32, 52, 62, 127,  62, 52, 32, 0,  0,  127,  127,  6,  12, 2,  127,  0,  127,  127,  73, 73, 65, 0,  126,  126,  25, 41, 78, 0,  126,  126,  25, 41, 78, 0,  3,  7,  124,  120,  4,  3,  0,  0,  0,  62, 127,  65, 65, 34, 0,  127,  127,  8,  8,  127,  0,  126,  126,  25, 41, 78, 0,  65, 127,  127,  65, 0,  38, 111,  73, 73, 58, 0,  1,  1,  127,  127,  1,  1,  0,  127,  127,  6,  12, 2,  127,  0,  126,  127,  9,  9,  126,  0,  38, 111,  73, 73, 58, 0,  0,  0,  0,  32, 52, 62, 127,  62, 52, 32, 0};

// joyeux noel 
//byte message[] = {0,  32, 52, 62, 127,  62, 52, 32, 0,  33, 65, 65, 63, 1,  0,  62, 65, 65, 65, 62, 0,  3,  4,  120,  4,  3,  0,  127,  73, 73, 65, 0,  63, 64, 64, 64, 63, 0,  65, 54, 8,  54, 65, 0,  0,  0,  127,  6,  8,  48, 123,  0,  62, 65, 65, 65, 62, 0,  127,  73, 73, 65, 0,  127,  64, 64, 64, 0,  0,  0,  32, 52, 62, 127,  62, 52, 32, 0,  0,  0};


uint8_t  mainByte = 1;  // on donne la valeur  au section automatique = 1  ou   manuel  = 0  ou rien  2  pas d'interet

//Variables:
const uint8_t loopTime = 100; //10hz
uint32_t lastTime = loopTime;
uint32_t currentTime = loopTime;

//Comm checks
uint8_t watchdogTimer = 12;      //make sure we are talking to AOG
uint8_t serialResetTimer = 0;   //if serial buffer is getting full, empty it

//Parsing PGN
bool isPGNFound = false, isHeaderFound = false;
uint8_t pgn = 0, dataLength = 0;
int16_t tempHeader = 0;

//hello from AgIO
uint8_t helloFromMachine[] = { 128, 129, 123, 123, 5, 0, 0, 0, 0, 0, 71 };
bool helloUDP = false;
//show life in AgIO
uint8_t helloAgIO[] = { 0x80, 0x81, 0x7B, 0xEA, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x6D };
uint8_t helloCounter = 0;

uint8_t AOG[] = { 0x80, 0x81, 0x7B, 0xEA, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0xCC };

//The variables used for storage
uint8_t relayLo = 0, relayHi = 0;

uint8_t onLo = 0, offLo = 0, onHi = 0, offHi = 0 ;


uint16_t tableau = 0;
uint16_t boucle = 0;
uint8_t boom = 0 ;

const int buttonPin = A0;         // the number of the pushbutton pin
const int ledPin = LED_BUILTIN;  // the number of the LED pin

//End of variables
void setup() {




  Serial.begin(38400);  //set up communication
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB
  }
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);

tableau= sizeof(message)*10;

} //end of setup

void loop() {






  currentTime = millis();
  if (currentTime - lastTime >= loopTime) {  //start timed loop
    lastTime = currentTime;


    //avoid overflow of watchdogTimer:
    if (watchdogTimer++ > 250) watchdogTimer = 12;

    //clean out serial buffer to prevent buffer overflow:
    if (serialResetTimer++ > 20) {
      while (Serial.available() > 0) Serial.read();
      serialResetTimer = 0;
    }


    mainByte = 1;
    bool buttonState = digitalRead(buttonPin);

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == LOW) {
      // turn LED on:
      digitalWrite(ledPin, HIGH);
      if (boucle++ > tableau) boucle = 0;
      boom = message[boucle / 10];

    } else {
      // turn LED off:
      digitalWrite(ledPin, LOW);
      boom = 0;
    }


    //Send to AOG
    AOG[5] = (uint8_t)mainByte;
    AOG[9] = (uint8_t)boom; //onLo;
    AOG[10] = (uint8_t)~boom; //offLo;


    //add the checksum
    int16_t CK_A = 0;
    for (uint8_t i = 2; i < sizeof(AOG) - 1; i++)
    {
      CK_A = (CK_A + AOG[i]);
    }
    AOG[sizeof(AOG) - 1] = CK_A;

    Serial.write(AOG, sizeof(AOG));
    Serial.flush();   // flush out buffer
  }
  //}

  // Serial Receive
  //Do we have a match with 0x8081?
  if (Serial.available() > 4 && !isHeaderFound && !isPGNFound)
  {
    uint8_t temp = Serial.read();
    if (tempHeader == 0x80 && temp == 0x81)
    {
      isHeaderFound = true;
      tempHeader = 0;
    }
    else
    {
      tempHeader = temp;     //save for next time
      return;
    }
  }

  //Find Source, PGN, and Length
  if (Serial.available() > 2 && isHeaderFound && !isPGNFound)
  {
    Serial.read(); //The 7F or less
    pgn = Serial.read();
    dataLength = Serial.read();
    isPGNFound = true;


  }

  //The data package
  if (Serial.available() > dataLength && isHeaderFound && isPGNFound)
  {

    if (pgn == 200) // Hello from AgIO
    {
      helloUDP = true;

      Serial.read(); //Version
      Serial.read();

      if (Serial.read())
      {
        relayLo -= 255;
        relayHi -= 255;
        watchdogTimer = 0;
      }

      //crc
      Serial.read();

      helloFromMachine[5] = relayLo;
      helloFromMachine[6] = relayHi;

      Serial.write(helloFromMachine, sizeof(helloFromMachine));

      //reset for next pgn sentence
      isHeaderFound = isPGNFound = false;
      pgn = dataLength = 0;
    }
    else { //reset for next pgn sentence
      isHeaderFound = isPGNFound = false;
      pgn = dataLength = 0;
    }
  }
} //end of main loop
