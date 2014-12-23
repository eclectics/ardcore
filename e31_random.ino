//  ============================================================
//
//  Program: e31 Random
//
//  Description: Set of random functions
//  On Load:
//  I/O Usage:
//    A0: choice of random type
//    A1: range
//    A2: chance of flip (for rls)
//    A3: chance of trigger D0
//    D0: trigger on clock depend on A3
//    D1: digital noise
//    Clock In: 
//    DAC: 
//
//   Expander:
//   A4:
//   A5:
//   13:
//   11 (also used by DAC):
//
//  Created:  21 Dec 2014
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
//  commercial work, please contact me at applied-eclectics.org 
//
//  For more information on the Creative Commons CC BY-NC license,
//  visit http://creativecommons.org/licenses/
//
//  ================= start of global section ==================

//  constants related to the Arduino Nano pin use
const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;   
const int dacseq=12;
const int ctrlinterval=15; //ms between reading a control
const int triginterval=25; //ms for led to light
volatile int clkState;

int i;
byte dac;
int chance=0;
int trigchance=0;
int range=0;
unsigned int seq=0;
int step;
int val;
int b; //needs to be int so can be shifted
byte fn;
byte lastdacs[dacseq];
unsigned long time, ctrltime=0,trigtime=0;
byte k=0; //knob to read

void setup(){    
  // set up the digital outputs
  for (i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i],LOW);
  }
  
  // set up the 8-bit DAC output pins
  for (i=0;i<8;i++){
      pinMode(pinOffset+i,OUTPUT);
    digitalWrite(pinOffset+i,LOW);
  }


    //set up variables
    randomSeed(millis());
    seq=random(16384); //16 bit value
    fn=0;
    for (i=0;i<dacseq;i++){
        lastdacs[i]=0;
    }

     // set up clock interrupt
    attachInterrupt(0, isr, RISING);
    //Serial.begin(9600);
}
void loop(){
//range -- how many intervals can it jump?
//quantisation?
//chance of change
//size of change
//
    if (clkState==HIGH){
        clkState=LOW;
        lblswitch:

        switch (fn){
            case 0: //random
                dac=random(255);
                break;
            case 1: //rls
                b=bitRead(seq,0);
                if (chance>random(1024)){
                    b=b?0:1;
                }
                seq=seq>>1 | b<<15;
                dac=lowByte(seq);
                break;
            case 2: //random walk
                //just jumps around but stays closeish to where it already is
                step=random(64)-32;
                dac=dac+step;
                if (dac>255){
                    dac=255-(dac-255);
                }
                if (dac<0){
                    dac=-1*dac;
                }
               break;
            case 3: //shift register (covers all values before repeat)
                if (dac==0) dac=random(255); //0 kills the sequence
                b=bitRead(dac,7)^bitRead(dac,5)^bitRead(dac,4)^bitRead(dac,3);
                dac=(dac<<1)|b;
                break;
            case 4: //bounces around last dacseq notes
                dac= lastdacs[random(dacseq)];
                break;
            case 5: //flip a bit?
                b=random(8); //maybe base this on chance ie map(chance,0,1024,0,8), so can restrict range of change while leaving it random?
                dac=dac ^ 1<<b;
                break;
            case 6:case 7: //do something random
                fn=random(5);
                goto lblswitch;
                break;
            default:
                break;
       }
       if (fn!=4){
        lastdacs[(++i)%12]=dac;
       }

       if (trigchance>random(1024)) trigger();
       dacOutput(map(dac,0,255,0,range));
       //mesg();
   }

    time=millis();

    if (time>ctrltime){
        val=analogRead(k);
        switch (k){
            case 0:
                fn=val>>7;
                break;
            case 1://how many values do we want
                range=val>>2; //0-255
                break;
            case 2: //chance
                chance=val;
               break;
            case 3:
                trigchance=val;
               break;
        }
        ctrltime=time+ctrlinterval;
        k=++k%4;
    }
    if (trigtime && time>trigtime){
        digitalWrite(digPin[0],LOW);
        trigtime=0;
    }

    digitalWrite(digPin[1],random(2));//digital noise
}


    //  ===================== end of program =======================

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
void trigger(){
    digitalWrite(digPin[0],HIGH);
    trigtime=millis()+triginterval;
}
void mesg(){
    Serial.print(" k ");
    Serial.print(k);
    Serial.print(" fn ");
    Serial.print(fn);
    Serial.print(" range ");
    Serial.print(range);
    Serial.print(" chance ");
    Serial.print(chance);
    Serial.print(" seq ");
    Serial.print(seq,BIN);
    Serial.print(" dac ");
    Serial.println(dac);
}
