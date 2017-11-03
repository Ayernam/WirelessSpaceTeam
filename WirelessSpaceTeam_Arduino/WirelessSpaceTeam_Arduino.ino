#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define red A2
#define green A1
#define yellow A0
RF24 radio (9, 10);

//sequence of light colors output
char sequence [100];
//sequence of light colors input
char userInput [100];
//round number
int roundNum = 1;
//number of latest light input
int lightNum = 1;

//stores data about current state of game
struct data {
  char color =  ' ';
  bool pass = 2;
};
data info;

//initialize radio
void initRadio() {
  radio.setChannel(18);
  radio.setPAlevel(RF24_PA_MIN);
  radio.openReadingPipe(0, 0xc2c2c2c2c2);
  radio.openWritingPipe(0xe7e7e7e7e7);
  radio.setCRCLength(RF24_CRC_16);
}

void setup() {
  Serial.begin(9600);
  radio.begin();
  initRadio();

  pinMode(red, INPUT);
  pinMode(yellow, INPUT);
  pinMode(green, INPUT);

  radio.startListening();
}

void loop() {
  //read data from Teensy
  if (radio.available()) {
    radio.read((char*) &info, sizeof(info));
    sequence[roundNum - 1] = info.color;
  }

  //take in input
  while (lightNum <= roundNum) {
    if (digitalRead(red)) {
      userInput[lightNum - 1] = 'r';
      lightNum++;
    }
    else if (digitalRead(green)) {
      userInput[lightNum - 1] = 'g';
      lightNum++;
    }
    else if (digitalRead(yellow)) {
      userInput[lightNum - 1] = 'y';
      lightNum++;
    }
  }

  //check if inout sequence is correct
  for (int i = 0; i < roundNum; i++) {
    if (sequence[i] != userInput[i]) {
      info.pass = false;
    }
  }
  //if input correct, move on; if not, reset game
  if (info.pass) {
    roundNum++;
  } else {
    roundNum = 1;
  }
  
  radio.stopListening();
  radio.write((char*) &info, sizeof(info));
  radio.startListening();
}
