//  Program: e26_rndtrig
//
//  Description: random triggers on clock; chance is chance on A0 if not then A1
//
//  I/O Usage:
//    A0: Chance of trigger d0 ccw is 0, cw is 100%. D1 is always !d0
//    A1: 
//    A2 In: chance of change for rand looping seq on analog out
//    A3 In: scale for rls
//    D0 Out: trigger 0
//    D1 Out: trigger 1
//    Clock In:  clock
//    Analog Out: Turning Machine like looping sequencer, also using A0 for chance of change
//
//  Input Expander: unused
//  Output Expander:  unused
//
//  Created:  10 Dec 2014
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
byte knob[]={0,0,0,0}; //using 0-255


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
        knob[0]=analogRead(A0)>>2;
        //knob[1]=analogRead(A1)>>2; //not used
        knob[2]=analogRead(A2)>>2;
        knob[3]=analogRead(A3)>>2;
         
        //random triggers; either D0 or D1 will fire
        //chance controls chance of D0, 
    
            if (random(254)<knob[0]){
                digitalWrite(digPin[0],HIGH);
                timeon[0]=millis()+trigTime;
            } else {
                digitalWrite(digPin[1],HIGH);
                timeon[1]=millis()+trigTime;
            }

        //looping sequencer
          b= seq & 1;
          seq= seq >> 1;
          if (random(254)<knob[2]){ //chance of bit flipping
            b=~b;
          }
          seq |= (b<<15);
          dacOutput(map(lowByte(seq),0,255,0,knob[3]));
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

