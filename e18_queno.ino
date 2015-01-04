/* Quneo controller
 *
 * vertical sliders are cc1-4
 * horizontal sliders are cc11-8
 * rotary are cc6 & 7
 * long horiz is cc 5
 *  
 * quneo is setup as follows for pads
 * this mirrors the notes used to set leds
 * 24 26 28 30
 * 16 18 20 22
 *  8 10 12 14
 *  0  2  4  6
 * quneo pads are set for remote led control only
 * Pressure and x/y from queno are disabled in the preset so
 * as not to overwhelm the software serial
 *
 * Sequence is green light moving, set pads are red, orange when the sequence moves across them
 *
 * ardcore d0 is midi out, a2 is midi in
 * an uses clock
 *
 * sequencer runs top to bottom, cause that seems right
 */
//global
#include <SoftwareSerial.h>
//
//
// // set up a new serial port
    byte digPin[2]={3,4};
    byte pinOffset=5;
    volatile long clkState;

    SoftwareSerial midi =  SoftwareSerial(A2, 3);

    const byte noteon=144; //10010000 note on channel 1
    const byte noteoff=128;//10000000 note off channel 1 //can use noteon with velocity 0?
    const byte cc=176; //10100000 cc channel 1 (followed by cc# and value)

    byte triggers[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte pads[16] = {24,26,28,30,16,18,20,22,8,10,12,14,0,2,4,6}; //notes corresponding to pads
    byte s,lasts; //index

    long time,controltime,triggertime;
    int control=20; //ms
    int triggerlen=20;

    char c,n,v;
    byte buf[]={144,0,127};
    int dir;
    //cc 10110000=176, ccnumber, value
            //{1,3,5,7}, //
            //{17,19,21,23},
    //quneo grid goes from 0-31, odd is red, even is green, starts at bottom and goes up
    //so row/column isi 8*row+2*col
    //byte notes[]={30,42,44,66,76,58,40,82}; //for audio sequence
    byte i,x,y,xy;
    byte color; //
    byte running;

void setup() {
    //pinMode(rxPin, INPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);

    for (int i=0; i<8; i++) {
        pinMode(pinOffset+i, OUTPUT);
        digitalWrite(pinOffset+i, LOW);
    }

    midi.begin(31250);
    controltime=0;
    triggertime=0;

    s=0;
    running=false;
    ctrllight(34,1); //stop button
    ctrllight(35,0); //stop button
    dir=1;
    ctrllight(37,1); //forward direction
    ctrllight(36,0); //forward direction
    for (i=0;i<16;i++){
        ctrllight(pads[i],0);
        ctrllight(pads[i]+1,0);
     }
     ctrllight(pads[0],1);
     attachInterrupt(0,isr,RISING);
}

void loop() {
//http://www.instructables.com/id/Send-and-Receive-MIDI-with-Arduino/step10/Receive-MIDI-Messages-with-Arduino/
//quneo in remote drum mode; test without ardcore
//get lights to light in sequence
    //output
    time=millis();
    if (clkState==HIGH && running){
        clkState=LOW;
        //off
        s+= dir;
        s=(s>=0)?s%16:16+s;
        ctrllight(pads[lasts],0); //targetting the green leds, which are indep of the red ones
        if (triggers[s]){
            digitalWrite(digPin[1],HIGH);
            triggertime=time+triggerlen;
        }
        ctrllight(pads[s],1);
        lasts=s;
    }
    if (triggertime && time>triggertime){
        triggertime=0;
        digitalWrite(digPin[1],LOW);
        //if (midi.overflow()) midi.listen(); //clear buffer if overflow;
    }

    //read midi from quneo
    while (midi.available()>2){//midi are always 3 char
        c=midi.read();
        n=midi.read();
        v=midi.read();
        switch(c){
            case noteon:
                dacOutput(v?n:v);
                switch (n){
                    case 33: //rhombus (reset)
                        s=(dir>0)?15:0;
                        break;
                    case 34://stop
                        running=false;
                        ctrllight(34,1);
                        ctrllight(35,0);
                        break;
                    case 35: //run
                        running=true;
                        ctrllight(34,0);
                        ctrllight(35,1);
                        break;
                        //h triangles in pairs
                    case 36:
                        dir=-1;
                        ctrllight(36,1);
                        ctrllight(37,0);
                        break;
                    case 37:
                        dir=1;
                        ctrllight(36,0);
                        ctrllight(37,1);
                        break;
                    case 38:
                    case 39:
                    case 40:
                    case 41:
                    case 42:
                    case 43:
                        break;
                    case 46: //v triangles pairs
                    case 47:
                    case 48:
                    case 49:
                        break;
                    default:    //pads
                        if (n>=0 && n<=30) { //pads
                            x=(n/2)%4;
                            y=3-n/8;
                            xy=y*4+x;
                            triggers[xy]=!triggers[xy];
                            ctrllight(pads[xy]+1,triggers[xy]); //red when hit
                        } 
                        break;
                }
            case noteoff:
                break;
            case cc:
                switch(n){
                    case 1:
                    case 2:
                    case 3:
                    case 4: //v sliders
                        break;
                    case 5: //long h slider
                        break;
                    case 6: //rotaries
                    case 7:
                    case 8: //h sliders
                    case 9: 
                    case 10: 
                    case 11: 
                        break;
                    default: //lots of continuous controller messages
                        break;

                }
                break;
            default:
                break;
        }

    }
    //if (time>controltime){
     //   controltime=time+control;
}

void isr(){
    clkState=HIGH;
}

void dacOutput(byte v)
{
  PORTB = (PORTB & B11100000) | (v >> 3);
	PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}
void ctrllight(byte ctrl,byte state){
//send light on/off with given note value
//state can be 0-3 for different brightness
        buf[0]=state?noteon:noteoff;
        buf[1]=ctrl;
        buf[3]=state?state<<6:0; //0,64,127
        midi.write(buf,3);

}
