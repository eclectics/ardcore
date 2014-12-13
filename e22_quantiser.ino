//  ============================================================
//
//  Program: e22  quantiser
//
//  Description: 0-5V gives 5 octaves ie 60 notes, but have 255 voltage levels to work with

//  I/O Usage:
//    Knob 1: scale selction 
//    Knob 2: range of octaves
//    Analog In 1: cv to be quantised -- full cw for no attenuation, attenuating it will alter the range, but not in octave increments
//    Analog In 2:  
//    Digital Out 1: LED lights when have a new quantised note
//    Digital Out 2: 
//    Clock In: 
//    Analog Out: quantised cv 
//
//  Input Expander:  not used
//  Output Expander: 
//
//  Created:  15 November 2014
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
//  commercial work, please contact 20 Objects LLC at our website
//  (www.20objects.com).
//
//  For more information on the Creative Commons CC BY-NC license,
//  visit http://creativecommons.org/licenses/
//
//  ================= start of global section ==================

//  constants related to the Arduino Nano pin use
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;   
unsigned long time,lastctrl;

//notes are just 0-255 going up in 1/60 increments rounded. 1/60 because 5 octaves of 12 notes each
//byte notes[]={0,4,9,13,17,21,26,30,34,38,43,47,51,55,60,64,68,72,77,81,85,89,94,98,102,106,111,115,119,123,128,132,136,140,145,149,153,157,162,166,170,174,179,183,187,191,196,200,204,208,213,217,221,225,230,234,238,242,247,251,255};

//thanks to dashwood for the following vales, which sound better
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
  int scales[]={07777, //chromatic
              05325, //major 101 011 010 101 5325
              05532,     //minor natural 101 101 011 010  5532
              04420,  //minor arpeggio
              04422,  //minor7th arpeggio
              04220,  //major arpeggio
              04221,  //major7th arpeggio 
              02452    //pentatonic 010 100 101 010 2452
            };

  byte note=0; 
  int readnote=0,lastnote=0; //note read
  byte selscale;  //holds index of currently seleected scale
  byte scale[61]; //this holds the notes we select out of the full range for the current scale
  byte scalenotes; //how many notes in the scale
  byte topscale=sizeof(scales)/2-1; //sizeof returns bytes, but scales are ints. This just represents the highest array index in scales
  int ctrlTime=30; //ms -- how often to we want to read the knobs, lower is more often
  byte range;//number of octaves
  byte numnotes; 
  int i,j;
  byte lastscale=0;
 
void setup(){
    
  // set up the digital outputs
  for (i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
  
  // set up the 8-bit DAC output pins
  for (i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
}
void loop(){
    //check for changes in a1/a2 periodically
    time=millis();
    if (time>lastctrl+ctrlTime){
      lastctrl=time;
      selscale=map(analogRead(A0),0,1023,0,topscale);
      range=map(analogRead(A1),0,1023,1,5); //how many octaves
      if (lastscale!=selscale){ //pick the notes for the scale
          lastscale=selscale;
          for(i=0,j=0;i<=60;i++){ //looking at this now, i'm all WTF? no doubt there's a clearer way
                                    //we're just reading the scale info note by note and picking notes from the full set
            if (scales[selscale] & (04000>>i%12)){ //should this interval be in the selected scale
              scale[j++]=notes[i];
            }
          }
          scalenotes=0; //how many notes per octave in this scale?
          i=scales[selscale];
          while (i){
             scalenotes+= i & 1; 
             i=i>>1;
          }
      }
       numnotes=scalenotes*range;
    }

    readnote=deJitter(analogRead(A2),readnote);

    if (lastnote!=readnote) {
      lastnote=readnote;
      note=map(readnote,0,1023,0,numnotes-1);
      dacOutput(scale[note]);
      digitalWrite(digPin[0],HIGH); //indicate change of note 
      delay(20);
      digitalWrite(digPin[0],LOW);
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

int deJitter(int v, int test) {
// this routine just make sure we have a significant value
// change before we bother implementing it. This is useful
// for cleaning up jittery analog inputs.
       if (abs(v - test) > 4) {
           return v;
       }
       return test;
}
//  ===================== end of program =======================
