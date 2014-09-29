//  ============================================================
//
//  Program: e11 Multifunction
//
//  Description: Multifunction sketch
//
//  I/O Usage:
//    A0: Clock rate
//    A1: 
//    A2 In: Sample Source (boosted by knob)
//    A3 In: 
//    D0 Out: Trigger on output
//    D1 2: unused
//    Clock In: External sample trigger; clock can be removed and clock time is maintained
//    Analog Out: sample out
//
//  Input Expander: unused
//  Output Expander: 7 is clock rate, 6 is /2, 5 is /4, etc up to 0 is /256
//
// Function Map
// 32 functions AND (distinctuishes 2/3 and 6/1, mult doesnt, so gives me coordinates)
// no 0s; want everything read out, so using 0 to 7 on A0/A1 as bits, not values
//    0   1   2   3   4   5   6   7
// 0  1   3   5   9   17  33  65  129
// 1  3   2   6   10  18  34  66  130
// 2  5   6   4   12  20  36  68  132
// 3  9   10  12  8   24  40  72  136   
// 4  17  18  20  24  16  48  80  144
// 5  33  34  36  40  48  32  96  160  
// 6  65  66  68  72  80  96  64  192
// 7  129 130 132 136 144 160 192 128 
//
//  Created:  29 March 2014
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
byte fn;
volatile int clkState;
//  constant for actual 0-5V quantization (vs. >> 4)


void setup(){
    pinMode(clkIn, INPUT);  
  
  // set up the 8-bit DAC output pins
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
  // set up clock interrupt
 // attachInterrupt(0, isr, RISING);

  bitSet(fn,analogRead(A0)>>7);
  bitSet(fn,analogRead(A1)>>7);
  dacOutput(fn);delay(100);dacOutput(0);delay(100); dacOutput(fn);delay(100); 

}
//loop just passes control to the function
void loop(){
  switch (fn) {
        //------------------------------------- 0 based
   case 1: //00
      select(); //shows value for a0/a1
      break;
    case 3: //01
    case 5: //02
    case 9: //03
    case 17: //04
    case 33: //05
    case 65: //06
    case 129://07
       select();
       break;
        //------------------------------------- 1 based
     case 2: //11
     case 6://12
     case 10://13
     case 18: //14
     case 34: //15
     case 66:  //16
     case 130: //17
       select();
       break;
        //------------------------------------- 2 based -- FUNCTIONS
     case 4: //22
       quantiser();
       break;
     case 12: //23 
       env();
       break;
     case 20: //24
     case 36: //25 
     case 68: //26
     case 132://27
       select();
       break;
       //quantiser
       //random gate
       //cv recorder (based onclock; use gate from beatstep?)
       //beatstep -- a2 for cv, clock for gate
       //level meter- use an input lieke a4/a5
       //d0= +ve, d1 for -ve
       //show output level on the dac
        //------------------------------------- 3 based CLOCK
      case 8: //33 
        clock();
        break;
     case 24: //34
        clockdivider();
        break;
     case 40: //35
        rcd();
        break;
     case 72://36
         pb();
         break;
     case 136://37
        select();
        break;
        //------------------------------------- 4 based  (LOGIC)
      case 16: //44
          comparator();
          break;
      case 48://45
          dlogic();
          break;
      case 80://46
         sr();
         break; 
      case 144://47
        select();
        break;
        //------------------------------------- 5 based SEQ
      case 32: //55
          rls();
          break;
      case 96: //56  
             sh5seq();
       break;
      case 160:
        select();
        break;
        //------------------------------------- 6 based
      case 64: //66
      case 192: //67
        select();
        break;
        //------------------------------------- 7 based        
      case 128: //77
        testfn(); //whatever I'm testing now
        break;
    default: //have I missed any?
      select(); 
      break;
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


void dpout(){
   for (int i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
}
void dpin(){
    pinMode(digPin[0], INPUT);
    pinMode(digPin[1], INPUT);
}

void bitslightshow(){
  while (1){
  for(int i = 0; i< 256; i++){
   dacOutput(i); 
   delay(analogRead(2)); 
  }
  }
}


void select(){
 //visual representation of A0, A1
  byte dac;
 
 while (1){
   dac=0;
   bitSet(dac,analogRead(A0)>>7);
   bitSet(dac,analogRead(A1)>>7);
   dacOutput(dac);
  delay(100);
 }
}




void quantiser(){
  //0-5V gives 5 ocataves
  //60 notes, but have 255 voltage levels to work with
  //what's best?
  //A0 for scale selection
  //A1 for range?
  //Use A2 for cv in
  //used cvnotes.js to generate intervals at 1/12v per note
  //need to restrict input between 0 and 5v
  byte notes[]={0,4,9,13,17,21,26,30,34,38,43,47,51,55,60,64,68,72,77,81,85,89,94,98,102,106,111,115,119,123,128,132,136,140,145,149,153,157,162,166,170,174,179,183,187,191,196,200,204,208,213,217,221,225,230,234,238,242,247,251,255};
  int scales[]={07777, //chromatic
              05235, //major 101 011 010 101 5325
              05532,     //minor natural 101 101 011 010  5532
              05531,      //minor harmonic 101 101 011 001 
              05646,      //heptatonic blues, lowered 357 101 110 100 110 5646 
              02452,    //pentatonic 010 100 101 010 2452
              04100,     //major 4ths? 100 001 000 000
              04221,     //major 3rds? 100 010 010 001
              04210,      //4ths 100 010 001 000 
              04444     // 3rds 100 100 100 100
            };
  long lastctrl,time;
  byte note,lastnote; //note read
  byte selscale=1; //major
  int seltran=0; //transposition
  byte scale[61];
  byte scalenotes=7;
  int ctrlTime=30; //ms
  byte range=5;//number of octaves
  byte numnotes=35;
  int i,j;
  lastnote=0;
  lastctrl=0;
  byte lastscale=0;
  dpout();
  
  while(1){
    //check for changes in a1
    time=millis();
    if (time>lastctrl+ctrlTime){
      lastctrl=time;
      selscale=map(analogRead(A0),0,1023,0,9);
      range=map(analogRead(A1),0,1023,1,5);
      if (lastscale!=selscale){
        lastscale=selscale;
          for(i=0,j=0;i<=60;i++){
            if (scales[selscale] & (04000>>i%12)){
              scale[j++]=notes[i];
            }
          }
          scalenotes=0; //how many notes in this scale?
          i=scales[selscale];
          while (i){
             scalenotes+= i & 1; 
             i=i>>1;
          }
      }
       numnotes=scalenotes*range;
    }
    note=map(analogRead(A2),0,1023,0,numnotes-1);
    
    if (lastnote!=note) {
      dacOutput(scale[note]);
      lastnote=note;
      digitalWrite(digPin[0],HIGH);
      delay(20);
      digitalWrite(digPin[0],LOW);
    }  
  }
}
void clock(){
  //DAC is a bunch of gates
  //d0 mirrors high bit of DAC
  //A0 controls rate -- cw for faster
  //add gate length -- so can go from trigger to gate
//poss?
//  Clock, based on internal timer.
//    option for syncing with external clock with the clock input
//   knobs --clock ,multiple /division
//    same, noon is clock, goes 1/2, 1/3 1/4, 1/5, 1/6, 2x, 3x 4x, 5x
//can I control the x value for each with the knobs?

    long int rate,time,high,dacrate;
    rate=map(analogRead(0),0,1023,128,0); //may want an exponential curve?, cause 1st half doubles rate, then next 1/4, then...
    time=6000*rate; //this controls slowest rate, experimental
    high=1; 
    dpout();
    
  while (1) {
    if (!high--){
        digitalWrite(digPin[0],LOW);
    }
  if (!time--){
      digitalWrite(digPin[0],HIGH);
      rate=map(analogRead(0),0,1023,128,0);
      time=20000*rate;
      dacrate=time/256;
      dac=255;
      dacOutput(dac);
      high=time/2; //for gate, for trigger use constant -- 300? /2 is pw
  }
  if(!dacrate--){
    dacOutput(--dac);
    dacrate=rate*20000/256;
  }  
  }
  
}

void clockdivider(){
 //clock in; this is the fastest clock
 //A1 is internal clock
 //A2 is division selector
 //A3 is trigger length; 0 for short - trigger, to 15 max; 90% of that clock
  attachInterrupt(0, isr, RISING);
  dpout();

  byte divisions[8][8]= {
      {2, 3, 4, 5, 6, 7, 8, 9}, //div 1-10
      {3, 5, 7, 9, 11, 13, 15, 17}, //div odd
      {3, 6, 9, 12, 15, 18, 21, 24}, //div three
      {2, 3, 4, 5, 6, 8, 10, 12}, //handy
      {2, 4, 8, 16, 32, 64, 128, 255}, //div power 2
      {2, 3, 5, 8, 13, 21, 34, 55}, //div fibonacci
      {2, 3, 5, 7, 11, 13, 17, 19}, //div prime
      {2,3,7,11,19,29,43,59} //whatever
  };
  
  const byte controlTime=0;
  const byte lastTime=1;
  const byte lastTrigger=2;
  const byte lastTick=3;
  const byte lastClock=4;
  const byte lastInternal=5;
  const byte internalClock=6;
  const byte minTrigger=7;
  const byte clockLength=8;
  const byte lastControl=9;

  
  byte sel=0;
  byte lastsel=0;
  byte counters[]={0,0,0,0,0,0,0,0};
  byte triggers[]={0,0,0,0,0,0,0,0}; //counters for gate length
  long timers[10];
  timers[controlTime]=100; //ms between reading pots
  timers[lastControl]=millis(); //last read of pots
  timers[lastTrigger]=millis();
  timers[lastTick]=millis(); //temp clock
  timers[lastInternal]=millis();
  timers[internalClock]=500; // period of internal clockto start
  timers[minTrigger]=30; //how long should the pulse last? ms
  timers[clockLength]=0;
  timers[lastClock]=millis();
  long time;
  byte i;
  int trigger; //%age of clock time for dac pulses
  byte dac;
  long f;
  int pw=40; //clock pulse length ms
  //reset input
  //puslse width?
  //temp use digpin1 as clock 250ms
  
  while(1){ 
    time=millis();
    
    if (time-timers[lastInternal]>timers[internalClock]){ //internal clock
       timers[lastInternal]=time;
       digitalWrite(digPin[1],HIGH);
    }
    if (time-timers[lastInternal]>timers[minTrigger]){
        digitalWrite(digPin[1],LOW);
    }
    
    if (time-timers[lastControl]>timers[controlTime]){
       timers[lastControl]=time;
       sel=analogRead(A2)>>7;
       trigger=analogRead(A3); 
       timers[internalClock]=100+analogRead(A1);
       //redo this with ints not floats
       if (trigger){
         for (i=0;i<8;i++){//pw is always a number of clocks
            triggers[i]=map(trigger,0,1023,1,divisions[sel][i]-1);
         }
       }

       //changing selection resets clocks
       if (sel!=lastsel){
         lastsel=sel;
         for (i=0;i<8;i++){
           counters[i]=divisions[sel][i];
         }
       }
    }
    
    //check triggers
    if (time-timers[lastTrigger]>timers[minTrigger]){ //timebased, based on ms as function of clock rate
       timers[lastTrigger]=time;
       digitalWrite(digPin[0],LOW);
    }
      
    if (clkState==HIGH){  //tick
        clkState=LOW;
        timers[clockLength]=time-timers[lastClock]; //keep clocklength in ms for triggerlength
        timers[lastClock]=time;
       digitalWrite(digPin[0],HIGH);

       dac=0;//pulses last 1 clock pulse unless trigger length
       for (i=0;i<8;i++){
           counters[i]--;
         if (trigger!=0 && divisions[sel][i]-counters[i]<triggers[i]){
            bitSet(dac,i);
         }
         if (counters[i]==0){
             bitSet(dac,i);
             counters[i]=divisions[sel][i]; //reset  
         }
  
       }
         dacOutput(dac);
    }
  }
  
}
//  ===================== Comparator =======================
// Compare a2/a3
//d0=1 when a2is <= a3 higher
//d1 is reverse of d0
//dac has the higher (or)
//not working??
//should use the -5->+5 pins? not 0-5?

void comparator(){
  int a2,a3;
  byte d0;
  dpout();
 while(1){
   a2=analogRead(2);
   a3=analogRead(3);
   if (a2>=a3){
     d0=1;
     dacOutput(a2>>2);
   } else {
     d0=0;
     dacOutput(a3>>2);
   }
   digitalWrite(digPin[0],d0);
   digitalWrite(digPin[1],!d0);
   delay(20); //no point doing this too fast?
 } 
}
//  ===================== Logic =======================
//use dac to output digital logic results
//use a2/a3 for input, then can bias them 
//d0/d1 show read state of a2/a3
//0 is 0
//1 is and
//2 is nand
//3 is or
//4 is nor
//5 is xor
//6 is ~d0
//7 is ~d1
//

void dlogic(){
  //booleans are stored as bytes, so ~ stuffs them up
  //mostly using logical instead of bitwise operators for shifting bits
  byte d0,d1,d; 
  dpout();
  while(1){
    d0=analogRead(2)>512; 
    d1=analogRead(3)>512;
    digitalWrite(digPin[0],d0);
    digitalWrite(digPin[1],d1);
    d= (!d1)<<7 | (!d0)<<6 | ((d0^d1)<<5)^0 |  (!(d0||d1))<<4 |(d0||d1)<<3 | (!(d0&&d1))<<2 | (d0&d1)<<1 ; 
    dacOutput(d);  
    delay(20);
  }
}

void rls(){
 //rotating shift register (random looping sequencer)
 // clock
 //A2 is chance of bit flipping
 //A1 is trigger time; but just for D0/D1; rest is a full clock pulse
 //D0 is clock
 //D1 is when bit changes
 //seq is 16 bits, only 8 of which are shown
 
    int trigTime=100;
    unsigned int seq;  //2 byte  value
    unsigned int randChance;
    byte b;
    byte lightsOn;
    long timeOn;
    
    dpout();
    attachInterrupt(0, isr, RISING);
    seq=random(65535); 
    
    while(1){
        if (clkState==HIGH){
          clkState=LOW;
          digitalWrite(digPin[0],HIGH);//clock

          lightsOn=HIGH;
          timeOn=millis();
      
          randChance=analogRead(A2);
          trigTime=10+(analogRead(A1)>>7)*15;
          b= seq & 1;
          seq= seq >> 1;
          if (random(0,1024) < randChance){
            b=~b;
            digitalWrite(digPin[1],HIGH);
          }
          seq |= (b<<15);
          dacOutput(lowByte(seq));

        }  
  
       if (lightsOn && (millis() - timeOn > trigTime)) {
        digitalWrite(digPin[0], LOW);
        digitalWrite(digPin[1], LOW);
        lightsOn=LOW;
      }
      
    }
  
}
//  Program: e05 sample and hold
//
//  Description: Sample and Hold last 5 notes, repeat 5 times
//
//  I/O Usage:
//    Knob 1: 
//    Knob 2: 
//    Analog In 1: Sample Source (boosted by knob)
//    Analog In 2: 
//    Digital Out 1: Trigger on output
//    Digital Out 2: unused
//    Clock In: External sample trigger; clock can be removed and clock time is maintained
//    Analog Out: sample out
//
//  Input Expander: unused
//  Output Expander: unused
//
//  Created:  29 March 2014
//  Modified: 
void sh5seq(){
  const int trigTime=100;
  byte notes[10]={0,0,0,0,0,0,0,0,0,0};
  byte n,i,repeat;
  boolean lightsOn;
  unsigned long time;
  int ledTime=105;
  
   dpout();
   attachInterrupt(0, isr, RISING);

  
  while(1){
       
      if (clkState==HIGH) {
        clkState=LOW;
        dacOutput(notes[n]);
        digitalWrite(digPin[0],HIGH);
        lightsOn=true;
        time=millis();
        notes[n+5]=map(analogRead(2),0,1024,0,255);
        n=++n%5;
        if (n==0){
          repeat=++repeat%5;
          digitalWrite(digPin[1],HIGH);
        }
        if (repeat==0){
          for (i=0;i<5;i++){
            notes[i]=notes[i+5];
          }
        }  
               
      }  
      
      if (lightsOn && millis()>time+ledTime){
        digitalWrite(digPin[0], LOW);
         digitalWrite(digPin[1], LOW);
         lightsOn=false;
      }
      
    
    
    
    
  }
  
}
//rotating clock divider

void rcd(){
  //a2 is divide, map to 7 rotations
  //a3 is reset - 5v+ for reset on next tick
  //clock input
  //a1 gate width as a % of clock time 
  byte divisions[]={1,2,3,4,5,6,7,8};
  byte counters[8]={1,2,3,4,5,6,7,8};
  attachInterrupt(0, isr, RISING);
  byte i;

  dpout();
  byte rot=0;
  byte dac;
  int pw; //ms
  boolean reset;
    
  while (1) {
    if (clkState==HIGH){
       clkState=LOW;
       
      //tick
        dac=0;
        for (i=0;i<8;i++){
          counters[i]--;
          if (counters[i]==0){
            dac=bitSet(dac,i);
            counters[i]=divisions[(i+rot)%8];
          }
        }
          dacOutput(dac);
        //read rotation and reset
          rot=map(analogRead(A2),0,1023,0,7);
          reset=analogRead(A3)==1023;
          pw=10+(analogRead(A1)>>7)*15; //10-130ms
          
          if (reset){
             for(i=0;i<8;i++){
              counters[i]=divisions[(i+rot)%8];
             }
          }
          delay(pw);
      }
      else {
          dacOutput(0);
      }
  }  
  
}

//shift register
void sr(){
 //cycling shift register
 //this funciton is supposed to give a cycle of 255 -- ie maximum length
//using x8 xor x6 xor x5 xor x4, rotate and add result to the x1
//a2 is rate; not using clock?
  int rate=250;
  int control=20; //ms how often to read clock
  long t;
  long lastt=0;
  long lastc=0;
  byte v=1;
  boolean b=0;
  dpout();
  
while(1){
  t=millis();
  if (t-lastt>rate){ //tick
      lastt=t;
      b=bitRead(v,7)^bitRead(v,5)^bitRead(v,4)^bitRead(v,3);
      v=(v<<1)|b;
      digitalWrite(digPin[0],b);
      dacOutput(v);
  }
  if (t-lastc>control){//read rate control
      lastc=t;
       rate=2048>>map(analogRead(2),0,1024,1,8); //nice way of getting linear/log response from knob
  } 
}
  
  
  
}

void env(){
  //use a4 and a5 for fun
  attachInterrupt(0, isr, RISING);
  byte trigger=LOW;
  unsigned int adsr[5]={0,0,0,0,0}; //first element is stage we're at
  byte level=0; //voltage level= 0-255
  while (1){
      if (clkState==HIGH){
        clkState=LOW;
        trigger=HIGH;
      }
      //retrigger?
      if (trigger==HIGH){ //set time counters and sustain level
        adsr[1]=analogRead(A2);
        adsr[2]=analogRead(A3);
        adsr[3]=analogRead(A4)+512;
        adsr[4]=analogRead(A5)+512;
        adsr[0]=1; //allows retrigger
      }
      if (adsr[0]){
        level=255*adsr[adsr[0]]/255;
        dacOutput(level);
      }
  }
  
}

void pb(){
 //pulse burst
 //cv controls number of gates to come out when a clock received
 //number of pulses is random up to a2 with minimum set by a0 (1-8)
 //gate duration is random up to a3, with minimum set by a1; and that's the dureation of the spaces between as well
 //clock ignored while pulses in progress; though may want to change this
 //gates out d0, d1 is inverse while pulses in progress
  byte state; //0 waiting, 1 started, 2 running
  int durationmax,durationmin;
  byte pulses;
  dpout();
  digitalWrite(digPin[0],LOW);
  attachInterrupt(0, isr, RISING);

  while(1){
    
    if (clkState==HIGH){
       clkState=LOW;
       if (!state){
         state=1;
       }
    }
    
    if (state){ //running; ignore clock until finished
      durationmax=(analogRead(A3)>>3)*20; //0-2560ms
      durationmin=(analogRead(A1)>>3)*20;
      for (int i=random(1+analogRead(A0)>>7,1+analogRead(A2)>>7);i>0;i--){ //1-8 pulses
        dacOutput(i);
        digitalWrite(digPin[0],HIGH);
        digitalWrite(digPin[1],LOW);
        delay(random(durationmin,durationmax));
        digitalWrite(digPin[0],LOW);
        digitalWrite(digPin[1],HIGH);
        delay(random(durationmin,durationmax));
      }
      dacOutput(0);
      digitalWrite(digPin[1],LOW); 
      state=0;
    }
  }
}

void gateecho(){
  //cv echo delay
  //like s/h based on clock in
  //maybe just do it with a gate to start with
  
  int rate,repeat;
  byte cv,val;
  attachInterrupt(0, isr, RISING);

  while (1){
    
    if (clkState==HIGH){
     //ignore clock until finished
      rate=analogRead(A0);
      repeat=analogRead(A1)>>7; 
      cv=analogRead(A2)>>2; //8bit dac
      val=255;
      
      for (; repeat>0; repeat--){
       dacOutput(val); 
       delay(rate>>1);
       dacOutput(0);
       delay(rate>>1);
       val>>=1; //perhaps relate to repeats
      }
      clkState=LOW;
      
    }
    
    
  }
}
 void testfn(){
    //meadowphysics
     byte i,j,dac,controlling,line,cv2,cv3;
     int triggertime=400;
     long t,lasttrigger=0;
     int knobs[4];//state of knobs when go intro control for a line; 0 not used
     byte matrix[8][5]={ //each line has {counter,intial state, linked lines, rule,destination} 
           {8,8,155,7,2}, //link lines don't include self
           {7,7,0,6,0},
           {6,6,12,5,2},
           {5,5,128,4,5},
           {4,4,32,3,6},
           {5,5,0,2,5},
           {2,2,89,1,6},
           {3,3,0,0,0}
         };
     attachInterrupt(0, isr, RISING);
     controlling=0;
    dpout();
    //read knobs
    for(i=0;i<4;i++){
        knobs[i]=analogRead(i); //A0 is 0, etc...
    }

 
   while(1){
       t=millis();
       if (controlling){ //A0 is not full cw
           for (i=0;i<4;i++){//check knob changes so can show most recent
                 if (analogRead(i)>>2 != knobs[i]>>2){ //ignore small fluctuations, and ignore values while cycling through lines
                     knobs[i]=analogRead(i);  //remember new position
                     dac=0;
                     switch(i){
                          case 0:
                            if (knobs[i]>1000){
                              controlling=0;
                              dacOutput(255);
                              break;
                            } 
                            j=map(knobs[i],0,1023,0,9); //8 is spacer, 9 is running
                            if (j<8){ //8 is just space
                              line=j;
                              bitSet(dac,line);
                            }
                            break;
                          case 1:
                             matrix[line][1]=(knobs[i]>>7) +1; //1-8, shown as 0-7
                             bitSet(dac,matrix[line][1]-1);
                             break;
                          case 2: //links 
                             matrix[line][2]=knobs[i]>>2; //0-255
                             bitClear(matrix[line][2],line);//can't link to self
                             dac=matrix[line][2];
                             break;
                          case 3: //rules
                              matrix[line][3]=knobs[i]>>7; //0-7
                              bitSet(dac,matrix[line][3]);
                              break;
                     }
                     dacOutput(dac);
                 }
              }
           
       } else { //running
             if (clkState==HIGH){ //decrementing can result in -ve result
               digitalWrite(digPin[0],HIGH);
               clkState=LOW;
               dac=0;
               cv2=analogRead(A2)>>7;
               cv3=analogRead(A3)>>7;
               matrix[0][0]--; //decrement top row only
               for (i=0;i<8;i++){//check all lines
                   //matrix[i][0]--; //this is more fun?
                   if (matrix[i][0]==0){
                       bitSet(dac,i);
                       for (j=0;j<8;j++){ //decrement links      
                          if (bitRead(matrix[i][2],j)){ //control can't link to itself
                              matrix[j][0]--; //might end up ==0?
                              if (matrix[j][0]==0){
                                  bitSet(dac,j);
                              }
                           }
                        }
                        switch (matrix[i][3]){  //run rule
                          case 0: //no rule
                            break;
                          case 1: //dest increment
                            matrix[matrix[i][4]][0]++;
                            break;
                          case 2: //dest decrement
                            matrix[matrix[i][4]][0]--;
                            break;
                          case 3: //dest to max
                              matrix[matrix[i][4]][0]=8;
                              break;
                           case 4: //dest to min                         
                               matrix[matrix[i][4]][0]=1;
                                break;
                           case 5: //dest random
                               matrix[matrix[i][4]][0]=random(1,8);
                               break;
                           case 6:
                               matrix[matrix[i][4]][0]=cv2; //could also use cv to select the rule?
                               break;
                           case 7:
                               matrix[matrix[i][4]][0]=cv3;
                               break;
                           default:
                               break;
                      }   
                   }
               }
                                 
                //work out new state of counters, set dac appropriately
                for (i=0;i<8;i++){
                    if (matrix[i][0]>8){ //rolled over because of negative
                        bitSet(dac,i);
                        matrix[i][0]=matrix[i][1]-(255-matrix[i][0])%matrix[i][1];
                    }
                    if (matrix[i][0]==0){ //in case left here
                      bitSet(dac,i);
                      matrix[i][0]=matrix[i][1];
                    }
                }
                //output
                dacOutput(dac);    
            }
           
           
             if (t-lasttrigger>triggertime){//running, turn off pulse
                digitalWrite(digPin[0],LOW);
                lasttrigger=t;
                dacOutput(0);
                controlling=(analogRead(A0)>1000)?0:1;
              } 
                       
      }
               
         
   } //end while
   
   
 } //end fn
  


//  ===================== end of program =======================

