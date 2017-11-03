#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define red 9
#define yellow 8
#define green 7
RF24 radio(9, 10);

//sequence of light colors output
char sequence [100];
//round number
int roundNum = 1;
//stores data about current state of game
struct data {
  char color =  ' ';
  bool pass;
};
data info;

//initialize radio
void initRadio()
{
  radio.setChannel(18);
  radio.setPAlevel(RF24_PA_MIN);
  radio.openReadingPipe(0, 0xe7e7e7e7e7);
  radio.openWritingPipe(0xc2c2c2c2c2);
  radio.setCRCLength(RF24_CRC_16);
}

void setup() {
  Serial.begin(9600);
  radio.begin();
  initRadio();

  randomSeed(analogRead(0));

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  radio.startListening();
}

char getLight() {
  long randomNum = random(0, 3);
  if (randomNum == 0) {
    return 'r';
  }
  else if (randomNum == 1) {
    return 'g';
  }
  else {
    return 'y';
  }
}

void flashLED(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void loop() {
  //Flash lights (and get new light)
  sequence[roundNum - 1] = getLight();
  info.color = sequence[roundNum - 1];

  for (int i = 0; i < roundNum; i++) {
    if (sequence[i] == 'r') {
      flashLED(red, 1000);
    }
    else if (sequence[i] == 'g') {
      flashLED(green, 1000);
    }
    else if (sequence[i] == 'y') {
      flashLED(yellow, 1000);
    }
    delay(250);
  }

  //Send newest color
  radio.stopListening();
  radio.write((char*) &info, sizeof(info));
  radio.startListening();

  while (radio.available()) {
      radio.read((char*) &info, sizeof(info));
  }

  if (info.pass) {
    lightLED(green, 2000);
    roundNum++;
  } else {
    lightLED(red, 2000);
    roundNum = 1;
  }
  delay(1000);
}
