//  Program: e28_rls
//
//  Description: Random Looping Sequencer, with quantisation option and variable loop length
//
//  I/O Usage:
//    On startup A0 is used to choose behaviour of D0/D1
//      A0 ccw; follows TM change, otherwise is chance of change from 0-100%
//
//    A0: Scale choice, full ccw is unquantised, or choice of available scales 
//    A1:  quantisation range; from 1 to number of notes available 
//    A2 In: chance of change for rand looping seq 
//    A3 In: length of rls sequence 
//    D0 Out: trigger 0 on bit change (based on A2)
//    D1 Out: trigger 1
//    Clock In:  clock
//    Analog Out: optionally quantised low byte of loop
//
// because the tm will use up to 255 positions, we won't get much change if we quantise it after the fact? taking 255 into 16 might not be interesting. If instead we allowed range to go 255/128/64/32/16/8/4/2 and use that as what we're reading, we won't be smudging the difference?
//
//  Input Expander: unused
//  Output Expander:  unused
//
//  Created:  12 Dec 2014
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


//sequence is variable length so using an array to store it
//not rotating the array physically, just keeping different pointers to where we're at in the sequence
const byte seqlength=40;
byte seq[seqlength];
byte ss=0; //seq start/end
byte lowbyte;
int i,j;
const int trigTime=20; //trigger length in milliseconds
const int ctrlTime=10; //trigger length in milliseconds
unsigned int b; //bit to be flipped
volatile int clkState;
unsigned long timeon[]={0,0};
unsigned long m,ctrlread=0;
byte scale=0,range=0,chance=0,length=0;

byte notes[]={0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,161,165,169,173,177,181,185,189,193,197,201,205,209,214,218,222,226,230,234,238,242};
  //using a 12 digit binary number to represent in and out of scale, each digit corresponds to 1 note
  //It gets translated to an octal number because using binary representation is limited to 8 digits unfortunately
  //In octal each digit is 0-7 and represents 3 binary digits, So
  //do the note selection you want ie 101011010101 (c major/ keyboard white keys). Make sure there's 12 steps!
  //group into sets of 3 101 011 010 101
  //each group of three turns into an octal number according to
  //000=0,001=1, 010=2, 011=3, 100=4,101=5,110=6,111=7
  //stick a 0 on the front of this 4 digit number-- that makes it get interpreted as octal
  //add it to the set of scales below-- delete any you don't want
  //you can comment lines out if you might need them again by preceding with // (like this line) 
  //Just make sure each one is follwed by a comma except the last one
  // Don't take the existing ones seriously, I don't know much about music...

  //The scale is chosen by A0, so the more there are here, the more fiddly it is
int scales[]={0, //used for unquantised
              07777, //chromatic
              05325, //major 101 011 010 101 5325
              05532,     //minor natural 101 101 011 010  5532
              04420,  //minor arpeggio
              04422,  //minor7th arpeggio
              04220,  //major arpeggio
              04221,  //major7th arpeggio 
              02452    //pentatonic 010 100 101 010 2452
            };

  byte note=0; 
  byte curscale[61]; //this holds the notes we select out of the full range for the current scale
  byte scalenotes; //how many notes in the scale
  byte numnotes=255; 
  byte lastscale=0;
  byte topscale=sizeof(scales)/2-1; //sizeof returns bytes, but scales are ints. This just represents the highest array index in scales
  byte ctrl=0; //which knob to read each pass
  byte following=false;
  byte tout;
  byte triggerchance=0;

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
  

   //set up random sequence -- this makes sure the random sequence starts out different each time
   randomSeed(millis());
   //could be replaced with
   //seq[]={0,1,0,0,1,...}; with 32 entries
   for (i=0;i<seqlength;i++){
    seq[i]=random(2);
   }

  //set up triggers
  following=false;
  triggerchance=analogRead(A0)>>3; //0-127
  if (triggerchance<28){
    following=true;
  } else{
    triggerchance=triggerchance-28;
  }
  // set up clock interrupt
  attachInterrupt(0, isr, RISING);

}

void loop(){
    if (clkState==HIGH){ //stuff only happens on clock
      clkState=LOW;
      
        //looping sequencer
          b= seq[ss]; //low bit
          //triggers
          if (following){
              tout=1; //D1 will fire unless bit flips
          }
          else {
            tout=(random(100)<triggerchance)?0:1;
          }

          if (random(254)<chance){ //chance of bit flipping
            b=b?0:1;
            if (following) tout=0;//ie fire A0 on change
          }
          digitalWrite(digPin[tout],HIGH);
          timeon[tout]=millis()+trigTime;

          seq[ss]=b; //"rotate" flipped bit 
          ss=(ss+1)%length;//increment seq
          lowbyte=rlsbyte(); //work out current value of byte for position in sequence
          if (scale){
              note=curscale[map(lowbyte,0,255,0,range)];
          } else{
              note=map(lowbyte,0,255,0,range);
          }
          dacOutput(note);
        }  

        m=millis();
    //turn off triggers
        for (i=0;i<2;i++){
            if (timeon[i] && m>timeon[i]){
                digitalWrite(digPin[i],LOW);
                timeon[i]=0;
            } 
        }
   //reading controls in between clock pulses; read 1 every ctrltime
   if (m>ctrlread){
    switch(ctrl){
        case 0: //changing scale
         scale=map(analogRead(A0),0,1023,0,topscale); //scale 
         if (lastscale!=scale){ //pick the notes for the scale
              lastscale=scale;
              if (scale){ //ie not 0
                  for(i=0,j=0;i<61;i++){ //we're just reading the scale info note by note and picking notes from the full set
                    if (scales[scale] & (04000>>i%12)){ //should this interval be in the selected scale
                      curscale[j++]=notes[i];
                    }
                  }
                  scalenotes=0; //how many notes per octave in this scale?
                  i=scales[scale];
                  while (i){
                     scalenotes+= i & 1; 
                     i=i>>1;
                  }
               numnotes=scalenotes*5-1;
              } else {
                  numnotes=255; //unquantised
              } 
          }
          break;
        case 1:
         range=map(analogRead(A1),0,1023,0,numnotes); //range notes
         break;
        case 2:
         chance=analogRead(A2)>>2; //TM flip chance
         break;
        case 3:
         length=map(analogRead(A3),0,1023,8,40); //cycle length from 8 to 40 (32 positions)
         break;
    }
    ctrl=(ctrl+1)%4;
    ctrlread=m+ctrlTime;
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

byte rlsbyte(){
    // if sl was constant can use this
        //lowbyte=lowbyte>>1;
        //lowbyte=lowbyte | seq[(ss+7)%sl]<<7
    // but if sl reduces may no longer be in the right zone, so will need sudden seq jump 
    // could just use above anyway, it would sort it self out within 7 clock pulses? might be a good segue?
 byte b=0;
    for (i=0;i<8;i++){
         b=b|seq[(ss+i)%length]<<i;
    }
    return b;
 }


//  ===================== end of program =======================

