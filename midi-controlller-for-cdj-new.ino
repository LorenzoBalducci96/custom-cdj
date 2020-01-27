#include <Control_Surface.h>

USBMIDI_Interface midi;

CCAbsoluteEncoder jogwheel = {
  {9, 10},       // pins
  MIDI_CC::Pan, // MIDI address (CC number + optional channel)
  1,            // optional multiplier if the control isn't fast enough

PBPotentiometer tempopitch = {
  A0,        // Analog pin connected to potentiometer
  CHANNEL_1, // MIDI Channel 1

// The note numbers corresponding to the buttons in the matrix
const AddressMatrix<4, 3> addresses = {{
  {47, 46, 45}, // Ableton drumrack map
  {44, 43, 42},
  {41, 40, 39},
  {38, 37, 36},
}};

NoteButtonMatrix<4, 3> buttonmatrix = {
  {2, 3, 4, 5}, // row pins
  {6, 7, 8},    // column pins
  addresses,    // address matrix
  CHANNEL_3,    // channel and cable number
};

void setup() {
  Control_Surface.begin();
}

void loop() {
  Control_Surface.loop();
}

