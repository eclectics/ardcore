//  Program: e26_rndtrig
//
//  Description: random triggers on clock
//
//  I/O Usage:
//    A0: Chance of trigger 0 (0-15)
//    A1: Chance of trigger 1 (0-15)
//    A2 In: gate length 0 from trigger (full ccw up to 8 clock pulses, random)
//    A3 In:  gate length 1
//    D0 Out: trigger 0
//    D1 2: trigger 1
//    Clock In:  clock
//    Analog Out: Turning Machine like looping sequencer, also using A0 for chance of change
//
//  Input Expander: unused
//  Output Expander: 7 is clock rate, 6 is /2, 5 is /4, etc up to 0 is /256
//
//  Created:  8 Dec 2014
//  Modified: 
//
//  ============================================================
//
//  License:
//
//  This software is licensed under the Creative Commons
//  "Attribution-NonCommercial license. This license allows you
//  to tweak and build upon the code for non-commercial purposes,
//  without the requirement to license derivative works on the
//  same terms. If you wish to use this (or derived) work for
//  commercial work, please contact  the author.
//
//  For more information on the Creative Commons CC BY-NC license,
//  visit http://creativecommons.org/licenses/
//
//  ================= start of global section ==================

//  constants related to the Arduino Nano pin use
// Arduino framework defines A0, A1, A2, A3 to refer to relevant pins
const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;   
byte dac;
byte i;
const int trigTime=20; //trigger length in milliseconds
unsigned int seq; //16 bit sequence, low 8 feeds dac
unsigned int b; //bit to be flipped
volatile int clkState;
unsigned long timeon[]={0,0};
unsigned long m;
byte chance[]={0,0,0}; //rand chance read from a0/a1, each 0-255


void setup(){
    pinMode(clkIn, INPUT);  
  
  //d0 and d1 are out only
  for (int i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }

  // set up the 8-bit DAC output pins
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
 // set up clock interrupt
 attachInterrupt(0, isr, RISING);
    
seq=random(65536); //random 16 bit value, showing low 8 only

}
void loop(){
    if (clkState==HIGH){ //stuff only happens on clock
          clkState=LOW;

        //reread controls
        chance[0]=analogRead(A0)>>2;
        chance[1]=analogRead(A1)>>2;
        chance[2]=analogRead(A2)>>2;
         
        //random triggers
        for (i=0;i<2;i++){
            if (random(254)<chance[i]){
                digitalWrite(digPin[i],HIGH);
                timeon[i]=millis()+trigTime;
            }
        }

        //looping sequencer
          b= seq & 1;
          seq= seq >> 1;
          if (random(254)<chance[2]){ //chance of bit flipping
            b=~b;
          }
          seq |= (b<<15);
          dacOutput(lowByte(seq));
        }  

        m=millis();
        for (i=0;i<2;i++){
            if (timeon[i] && m>timeon[i]){
                digitalWrite(digPin[i],LOW);
                timeon[i]=0;
            } 
        }
  
}

//  ==================== additional routines ===================

//  dacOutput(byte) - deal with the DAC output
//  -----------------------------------------
void dacOutput(byte v)
{
  PORTB = (PORTB & B11100000) | (v >> 3);
	PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}
//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
  clkState = HIGH;
}



//  ===================== end of program =======================

