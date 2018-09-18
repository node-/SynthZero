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
#define BUTTON1_PIN 1
#define BUTTON2_PIN 4
#define LED_PIN 12

Oscil<CHUM9_NUM_CELLS, AUDIO_RATE> aCrunchySound(CHUM9_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);

int val = 0;
LowPassFilter lpf;
int button1 = 0, button2 = 0;
bool b1 = 0, b2 = 0;
float freq = 255; // 0-255
int pot_min = 255, pot_max = 0;


void setup(){
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  startMozzi(CONTROL_RATE);
  aCrunchySound.setFreq(0.01f);
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
    b1 = button1 == HIGH;
    b2 = button2 == HIGH;
    val = analogRead(POT_PIN);

    if (val < pot_min)
      pot_min = val;
    if (val > pot_max)
      pot_max = val;

    freq = ((float)(val - pot_min)*255/pot_max); // from 0 to 255
    kFilterMod.setFreq(freq);  // choose a new modulation frequency
    
    /*
    Serial.print("freq: ");
    Serial.println(freq);
    Serial.print("b1: ");
    Serial.println(b1);
    Serial.print("b2: ");
    Serial.println(b2);
    Serial.print("pot_min: ");
    Serial.println(pot_min);
    Serial.print("pot_max: ");
    Serial.println(pot_max);
    Serial.println("");
    */
  }
  if (b2) {

    // map the modulation into the filter range (0-255)
    //byte cutoff_freq = 100 + kFilterMod.next()/2;
    
    byte cutoff_freq = ( freq );
    lpf.setCutoffFreq(cutoff_freq);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

int updateAudio(){
  char asig = lpf.next(aCrunchySound.next());
  return (int) asig;
}