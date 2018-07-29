#include "MIDIUSB.h"
#include "PitchToNote.h"
#include "RotaryEncoder.h"
#define NUM_BUTTONS  12

const uint8_t button1 = 2;
const uint8_t button2 = 3;
const uint8_t button3 = 4;
const uint8_t button4 = 5;
const uint8_t button5 = 6;
const uint8_t button6 = 7;
const uint8_t button7 = 8;
const uint8_t button8 = 9;
const uint8_t button9 = 10;
const uint8_t button10 = 11;
const uint8_t button11 = 12;
const uint8_t button12 = 13;

const int intensityPot = A5;  //tensione pitch
const int controlPin = A4;//tensione contorllo pitch
const int jogUnoPin = A2;
const int jogDuePin = A3;
const int pinTastiera = A1;
const int pinTempo = A0;

unsigned int CENTERVALUE = 512;

const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};
const byte notePitches[NUM_BUTTONS] ={0x01,    0x02,    0x03,    0x04,    0x05,    0x27,    0x07,    0x08,    0x09,    0x10,     0x11,     0x12};//0x27 is the code for the jogwheel scratch

bool premuti[NUM_BUTTONS] = {   false,   false,   false,   false,   false,   false,   false,   false,   false,   false,    false,    false};
bool premutiInPrecedenza[NUM_BUTTONS] = {false,   false,   false,   false,   false,   false,   false,   false,   false,   false,    false,    false};
bool premutiTastiera[6] = {   false,   false,   false,   false,   false,   false};
int tastoPremuto = 0;
int tastoPrecedentementePremuto = 0;
int tempoPremuto = 0;
int tempoPrecedentementePremuto = 0;
bool primaLetturaButtata = false;
bool primaLetturaTempoButtata = false;

const uint8_t jogControlValue = 0x27;

int salendoScendendo = 0;
unsigned int valorePitchPrecedente = 0;
unsigned int valorePitchMappato = 0;

unsigned int valorePitch = 0;
unsigned int valoreControl = 0;


uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

RotaryEncoder encoder(jogUnoPin,jogDuePin,1,1,1000);
int rotaryValue = 0;

void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(intensityPot, INPUT);
    pinMode(controlPin, INPUT);
    pinMode(jogUnoPin, INPUT);
    pinMode(jogDuePin, INPUT);
  Serial.begin(9600);
    
}


void loop() {
  readButtons();
  readControl();
  readIntensity();
  readRotary();
  playNotes();
}

void readButtons()//analizziamo i bottoni
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      premuti[i] = 1;//registra nell'array dei bottoni che e stato premuto (scrive 1) il tasto i
      //Serial.println("letto:");
      //Serial.println(i);
    }
    else
      premuti[i] = 0;//registra nell'array dei bottoni che non e stato premuto (scrive 0)il tasto i
  }

 
  int valoreTastierino = analogRead(pinTastiera);
  //Serial.println(valoreTastierino);
  if(valoreTastierino < 50)
    tastoPremuto = 1;
  else if(valoreTastierino < 230 && valoreTastierino > 185)
    tastoPremuto = 2;
  else if(valoreTastierino < 395 && valoreTastierino > 345)
    tastoPremuto = 3;
  else if(valoreTastierino < 545 && valoreTastierino > 495)
    tastoPremuto = 4;
  else if(valoreTastierino < 690 && valoreTastierino > 640)
    tastoPremuto = 5;
  else if(valoreTastierino < 845 && valoreTastierino > 795)
    tastoPremuto = 6;
  else
    tastoPremuto = 0;

  int valoreTempo = analogRead(pinTempo);
  if(valoreTempo < 50)
    tempoPremuto = 1;
  else if(valoreTempo < 230 && valoreTempo > 185)
    tempoPremuto = 2;
  else if(valoreTempo < 395 && valoreTempo > 345)
    tempoPremuto = 3;
  else if(valoreTempo < 545 && valoreTempo > 495)
    tempoPremuto = 4;
  else if(valoreTempo < 690 && valoreTempo > 640)
    tempoPremuto = 5;
  else if(valoreTempo < 845 && valoreTempo > 795)
    tempoPremuto = 6;
  else
    tempoPremuto = 0;
  //Serial.println(tempoPremuto);
}

void readIntensity()//analizziamo il pitch
{
  valorePitch = analogRead(intensityPot);
}

void readControl(){//analizziamo il valore del ct del pitch
  valoreControl = analogRead(controlPin);
}

void readRotary(){
  rotaryValue = encoder.readEncoder();
}

void pitchChange(unsigned int pitch){
    uint8_t setteBitPiuSignificativi = 0;
    uint8_t setteBitMenoSignificativi = 0;
    
    while(pitch >= 128){
      setteBitPiuSignificativi++;
      pitch = pitch - 128;
    }
    setteBitMenoSignificativi = (uint8_t) pitch;
    midiEventPacket_t pitchbend = {0xE, 0xE0 | 0, setteBitMenoSignificativi, setteBitPiuSignificativi};
    MidiUSB.sendMIDI(pitchbend);
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (premuti[i] != premutiInPrecedenza[i])//se c'e stato un aggiornamento dei tasti che sono stati premuti / rilasciti...
    {
      if (premuti[i])//se e stato premuto...
      {
        premutiInPrecedenza[i] = true;//registra su i tasti premuti precedentemente che e stato premuto (evita che lo reinvio un' altra volta
        //Serial.println("hai premuto");
        //Serial.println(i);
        noteOn(0, notePitches[i], 0);
        MidiUSB.flush();
      }
      else
      {
        premutiInPrecedenza[i] = false;
        //Serial.println("hai lasciato");
        //Serial.println(i);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }

  //invio jog
  
  if(rotaryValue != 0){
    //Serial.println("mando rotary");
    //64 is 0x40 that is the center jog value
    uint8_t valueToSend = 64 - rotaryValue;
    controlChange(0, jogControlValue, valueToSend);
    MidiUSB.flush();
  }

  //check e invio del pitch
  if(abs(valorePitch - valorePitchPrecedente) > 1 || ((valorePitch - valorePitchPrecedente) > 0 && salendoScendendo > 0) || ((valorePitch - valorePitchPrecedente) < 0 && salendoScendendo < 0)){
    //Serial.println("mando pitch");
    //Serial.println(valorePitch);
    salendoScendendo = valorePitch - valorePitchPrecedente;
    valorePitchPrecedente = valorePitch;
    if(valorePitch < valoreControl)
      valorePitchMappato = (unsigned int) map(valorePitch, 0, valoreControl, 0, 8191);
    else//valorePitch >= valoreControl
      valorePitchMappato = (unsigned int) map(valorePitch, valoreControl, 1023, 8192, 16383);
    pitchChange(valorePitchMappato);
      MidiUSB.flush();
  }

  if(tastoPremuto != tastoPrecedentementePremuto){
    if(!primaLetturaButtata){
      primaLetturaButtata = true;
    }
    else{
      primaLetturaButtata = false;
      if(tastoPremuto == 0){
        noteOff(0, (0x20 + tastoPrecedentementePremuto), 0);
        MidiUSB.flush();
        //Serial.println("lascito");
        //Serial.println((tastoPrecedentementePremuto));
      }
      else{
        noteOn(0, 0x20 + tastoPremuto, 0);
        MidiUSB.flush();
        //Serial.println("premuto");
        //Serial.println(tastoPremuto);
      }
      tastoPrecedentementePremuto = tastoPremuto;
    }
  }

  if(tempoPremuto != tempoPrecedentementePremuto){
    if(!primaLetturaTempoButtata){
      primaLetturaTempoButtata = true;
    }
    else{
      primaLetturaTempoButtata = false;
      if(tempoPremuto == 0){
        noteOff(0, (0x50 + tempoPrecedentementePremuto), 0);
        MidiUSB.flush();
        //Serial.println("lascito");
        //Serial.println((tempoPrecedentementePremuto));
      }
      else{
        noteOn(0, 0x50 + tempoPremuto, 0);
        MidiUSB.flush();
        //Serial.println("premuto");
        //Serial.println(tempoPremuto);
      }
      tempoPrecedentementePremuto = tempoPremuto;
    }
  }
  delay(1);
}


