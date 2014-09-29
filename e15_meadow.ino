//  ============================================================
//
//  Program: e15 Meadow
//
//  Description: a meadow physics style gate/cv generator
//
//  I/O Usage:
//    A0: full cw to run the generator, otherwise to choose which line to work on
//    A1: set the reset value (1-8, shown as 0-7) 
//    A2 In: set the linked lines, while running cv in is reset value for a rule 
//    A3 In: set the rule, while running cv in is reset value for a rule 
//    D0 Out: display clock 
//    D1 2: unused
//    Clock In: decrements line 0
//    Analog Out: cv out
//
//  Input Expander: unused
//  Output Expander: DAC pin outs used for gates
//
//  Created:  28 Sept 2014
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
const int pinOffset = 5;       // the first DAC pin (from 5-12)

volatile int clkState;
//  constant for actual 0-5V quantization (vs. >> 4)
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
     

void setup(){
      pinMode(clkIn, INPUT);  
      pinMode(digPin[0],OUTPUT);

      // set up the 8-bit DAC output pins
      for (int i=0; i<8; i++) {
        pinMode(pinOffset+i, OUTPUT);
        digitalWrite(pinOffset+i, LOW);
      }

      // set up clock interrupt
      attachInterrupt(0, isr, RISING);

     controlling=0;

    //read knobs
    for(i=0;i<4;i++){
        knobs[i]=analogRead(i); //A0 is 0, etc...
    }

}
void loop(){
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
               
         
   
   
 } 
  

//  ==================== additional routines ===================

//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
  clkState = HIGH;
}

//  dacOutput(byte) - deal with the DAC output
//  -----------------------------------------
void dacOutput(byte v)
{
  PORTB = (PORTB & B11100000) | (v >> 3);
	PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}

//  ===================== end of program =======================
