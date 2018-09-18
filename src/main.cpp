/*  Example of filtering a wave,
    using Mozzi sonification library.
  
    Demonstrates LowPassFilter().
  
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or 
    check the README or https://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, CC by-nc-sa.
*/

//#include <ADC.h>  // Teensy 3.1 uncomment this line and install https://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <tables/cos2048_int8.h> // for filter modulation
#include <LowPassFilter.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 64 // powers of 2 please
#define POT_PIN 2
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

int val = 0;
LowPassFilter lpf;
bool button1 = 0, button2 = 0;

void setup(){
  Serial.begin(9600);
  startMozzi(CONTROL_RATE);
  aCrunchySound.setFreq(2.f);
  kFilterMod.setFreq(1.3f);
  lpf.setResonance(200);
}

void loop(){
  audioHook();
}

void updateControl(){
  if (rand(CONTROL_RATE/2) == 0){ // about once every half second
    button1 = digitalRead(BUTTON1_PIN);
    button2 = digitalRead(BUTTON2_PIN);
    val = analogRead(POT_PIN);
    kFilterMod.setFreq((float)val/64);  // choose a new modulation frequency
    
    Serial.print("val: ");
    Serial.println(val);
    Serial.print("val/64: ");
    Serial.println((float)val/64);
    Serial.print("b1: ");
    Serial.println(button1);
    Serial.print("b2: ");
    Serial.println(button2);
    Serial.println("");
  }
  if (button1 && button2) {
    // map the modulation into the filter range (0-255)
    //byte cutoff_freq = 100 + kFilterMod.next()/2;
    byte cuttoff_freq = ((float)val/64);
    lpf.setCutoffFreq(cutoff_freq);
  }
}

int updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return (int) asig;
}