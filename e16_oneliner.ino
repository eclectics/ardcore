/*
 * Formulas are from http://countercomplex.blogspot.com.au/search/label/algorithmic%20music
 * Linked from that page is the following http://pelulamu.net/countercomplex/music_formula_collection.txt  Collection of oneliner music formulas. Version 2011-10-18
 * Switch is using  list under  ====== ALL COLLECTED FORMULAS (length order) ======
 * Some are commented out, haven't fixed them for arduino yet
 * A0 is delay after each iteration; imho it sounds better with this about 1/4 turn cw
 * A2 selects the formula  -- there are a few gaps in the list though, so a few spots are silent. You can get rid of the breaks in those instances and it will fall through to the next one.
 * Controls are read on clock; so can just patch in a faster clock if want more responsive controls. This also allows sending a noise source to A2 and a slowish clock for a weird sequence. Or send it a gate and cv from a keyboard and have it selectable
*/

volatile int clkState;
byte fn,v;
const int iterations=2048; //how often to read
const int pulse=2000; //indication of clock pulse
int pause=1000; //want to slow it to 8k samples/sec or so, A0 sets this

//woah -- huge different between long and int; affects not only timing...
unsigned long i,o,t,p;
//unsigned int i,o,t,p;

void setup() {
    t=0;
    o=0;
    pinMode(3,OUTPUT);
    for (i=0; i<8; i++) {
        pinMode(5+i, OUTPUT);
        digitalWrite(5+i, LOW);
      }

    i=0;
    clkState=LOW;
    attachInterrupt(0, isr, RISING);
}

void loop() {
    t++;
    i++;
    switch(fn){
        case 0: // minimal sierpinski harmony
            o=t&t>>8;
            break;
        case 1: // "the 42 melody", separately discovered by several people on irc etc
            o=t*(42&t>>10);
            break;
        case 2:  // danharaj 2011-10-03 http://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/ "fractal trees", 216's version
            o=t|t%255|t%257;
            break;
        case 3:  // droid 2011-10-05 http://pouet.net/topic.php?which=8357&page=10
            o= t>>6&1?t>>5:-t>>4;
            break;
        case 4:  // Niklas_Roy 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
            o=t*(t>>9|t>>13)&16;
            break;
        case 5:  // krcko 2011-10-04 http://rafforum.rs/index.php/topic,123.0.html
            o= (t&t>>12)*(t>>4|t>>8);
            break;
        case 6: // viznut 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
            o=(t*5&t>>7)|(t*3&t>>10);
            break;
        case 7: // tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
            o=(t*(t>>5|t>>8))>>(t>>16);
            break;
        case 8:// miiro 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
            o= t*5&(t>>7)|t*3&(t*4>>10);
            break;
        case 9: // robert 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
            o= (t>>13|t%24)&(t>>7|t%19);
            break;
        case 10:// Niklas_Roy 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
            o=(t*((t>>9|t>>13)&15))&129;
            break;
        case 11: // viznut 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o  
            o=(t&t%255)-(t*3&t>>13&t>>6);
            break;
        case 12: // krcko 2011-10-04 http://rafforum.rs/index.php/topic,123.0.html
            o= (t&t>>12)*(t>>4|t>>8)^t>>6;
            break;
        case 13: // blueberry 2011-10-05 http://pouet.net/topic.php?which=8357&page=12 11kHz
            o= t*(((t>>9)^((t>>9)-1)^1)%13);
            break;
        case 14: // rrola 2011-10-04 http://pouet.net/topic.php?which=8357&page=9 optimized by ryg
            o=t*(0xCA98>>(t>>9&14)&15)|t>>8;
            break;
        case 15: // tonic 2011-10-01 http://pouet.net/topic.php?which=8357&page=5 "mr. arpeggiator playing a solo"
            o= (t/8)>>(t>>9)*t/((t>>14&3)+4);
            break;
        case 16: // // FreeFull 2011-10-12 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
			o=(~t/100|(t*3))^(t*3&(t>>5))&t;
            break;
        case 17:// // red- 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
			o=(t|(t>>9|t>>7))*t&(t>>11|t>>9);
            break;
        case 18:// // harism 2011-10-09 http://0xa.kuri.mu/2011/10/09/bitop-videos/
			o=((t>>1%128)+20)*3*t>>14*t>>18 ;
            break;
        case 19:// // droid 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
            //o= t&(sin(t&t&3)*t>>5)/(t>>3&t>>6);
            break;
        case 20:// // viznut 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
            o= t*(((t>>12)|(t>>8))&(63&(t>>4)));
            break;
        case 21:// // visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
            o= t*(((t>>9)|(t>>13))&(25&(t>>6)));
            break;
        case 22:// // 216 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
            o= t*(t^t+(t>>15|1)^(t-1280^t)>>10);
            break;
        case 23:// // tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
            o= t*(((t>>11)&(t>>8))&(123&(t>>3)));
            break;
        case 24:// // viznut 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y (xpansive+varjohukka)
			o=(t>>7|t|t>>6)*10+4*(t&t>>13|t>>6);
            break;
        case 25:// // stephth 2011-10-03 http://news.ycombinator.com/item?id=3063359
			o=(t*9&t>>4|t*5&t>>7|t*3&t/1024)-1;
            break;
        case 26:// // visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg "Space Invaders vs Pong"
            o= t*(t>>((t>>9)|(t>>8))&(63&(t>>4)));
            break;
        case 27:// // viznut 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
			o=(t>>6|t|t>>(t>>16))*10+((t>>11)&7);
            break;
        case 28:// // yumeji 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
			o=(t>>1)*(0xbad2dea1>>(t>>13)&3)|t>>5;
            break;
        case 29:// // ryg 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
			o=(t>>4)*(13&(0x8898a989>>(t>>11&30)));
            break;
        case 30:// // marmakoide 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
			o=(t>>(t&7))|(t<<(t&42))|(t>>7)|(t<<5);
            break;
        case 31:// // robert 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
			o=(t>>7|t%45)&(t>>8|t%35)&(t>>11|t%20);
            break;
        case 32:// // lucasvb 2011-10-03 http://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/
			o=(t>>6|t<<1)+(t>>5|t<<3|t>>3)|t>>2|t<<1;
            break;
        case 33:// // bear @ celephais
            o= t+(t&t^t>>6)-t*((t>>9)&(t%16?2:6)&t>>9);
            break;
        case 34:// // xpansive 2011-09-29 http://pouet.net/topic.php?which=8357&page=2 "Lost in Space"
			o=((t*(t>>8|t>>9)&46&t>>8))^(t&t>>13|t>>6);
            break;
        case 35:// // rez 2011-10-05 http://pouet.net/topic.php?which=8357&page=11 js-only optimized by ryg
            //o= t*(1+"4451"[t>>13&3]/10)&t>>9+(t*0.003&3);
            break;
        case 36:// // marmakoide 2011-10-03 http://pouet.net/topic.php?which=8357&page=7 "Lemmings March"
			o=(t>>5)|(t<<4)|((t&1023)^1981)|((t-67)>>4);
            break;
        case 37:// // droid 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
            o=t>>4|t&(t>>5)/(t>>7-(t>>15)&-t>>7-(t>>15));
            break;
        case 38:// // rez 2011-10-03 http://pouet.net/topic.php?which=8357&page=7
            o= t*(t/256)-t*(t/255)+t*(t>>5|t>>6|t<<2&t>>1);
            break;
        case 39:// // viznut 2011-10-06 #countercomplex "moon scanner generalization", based on jaffa's formula
			o=((t>>5&t)-(t>>5)+(t>>5&t))+(t*((t>>14)&14));
            break;
        case 40:// // viznut 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
			o=(t*((3+(1^t>>10&5))*(5+(3&t>>14))))>>(t>>8&3);
            break;
        case 41:// // droid 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
            //o=t>>4|t&DIV((t>>5),(t>>7-(t>>15)&-t>>7-(t>>15)));
            break;
        case 42:// // pyryp 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
            o=(o>>1)+(o>>4)+t*(((t>>16)|(t>>6))&(69&(t>>9)));
            break;
        case 43:// // bst 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
			o=(int)(t/1e7*t*t+t)%127|t>>4|t>>5|t%127+(t>>16)|t;
            break;
        case 44:// // tangent128 2011-10-09 http://0xa.kuri.mu/2011/10/09/bitop-videos/
            o=t*(((t>>9)&10)|((t>>11)&24)^((t>>10)&15&(t>>15)));
            break;
        case 45:// // tejeez 2011-10-05 #countercomplex
			o=(~t>>2)*((127&t*(7&t>>10))<(245&t*(2+(5&t>>14))));
            break;
        case 46:// // lokori 2011-10-04 #suomiscene
			o=(t+(t>>2)|(t>>5))+(t>>3)|((t>>13)|(t>>7)|(t>>11));
            break;
        case 47:// // visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
            o=t*(t>>8*((t>>15)|(t>>8))&(20|(t>>19)*5>>t|(t>>3)));
            break;
        case 48:// // Aaron_Krister_Johnson 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
			o=(t>>4)|(t%10)|(((t%101)|(t>>14))&((t>>7)|(t*t%17)));
            break;
        case 49:// // jounim 2011-10-04 #suomiscene
			o=((t&((t>>5)))+(t|((t>>7))))&(t>>6)|(t>>5)&(t*(t>>7));
            break;
        case 50:// // spikey 2011-10-04 #suomiscene based on jounim's formula
			o=((t&((t>>23)))+(t|(t>>2)))&(t>>3)|(t>>5)&(t*(t>>7));
            break;
        case 51:// // akx 2011-10-05 http://twitter.com/#!/akx
			o=(((((t*((t>>9|t>>13)&15))&255/15)*9)%(1<<7))<<2)%6<<4;
            break;
        case 52:// // bst 2011-10-05 http://pouet.net/topic.php?which=8357&page=10
			o=((t%42)*(t>>4)|(0x15483113)-(t>>4))/(t>>16)^(t|(t>>4));
            break;
        case 53:// // skurk 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
            o=t*(t>>((t&4096)?((t*t)/4096):(t/4096)))|(t<<(t/256))|(t>>4);
            break;
        case 54:// // skurk+raer 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
			o=((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t));
            break;
        case 55:// // yumeji 2011-10-06 http://pouet.net/topic.php?which=8357&page=12 "badbeats & safe"
            o=t*((0xbadbea75>>((t>>12)&30)&3)*0.25*(0x5afe5>>((t>>16)&28)&3));
            break;
        case 56:// // bst 2011-10-11 http://pouet.net/topic.php?which=8357&page=18
            o=t>>16|((t>>4)%16)|((t>>4)%192)|(t*t%64)|(t*t%96)|(t>>16)*(t|t>>5);
            break;
        case 57:// // bear @ celephais
            o=t>>6^t&37|t+(t^t>>11)-t*((t%24?2:6)&t>>11)^t<<1&(t&598?t>>4:t>>10);
            break;
        case 58:// // kb 2011-10-04 http://pouet.net/topic.php?which=8357&page=8 44kHz
			o=((t/2*(15&(0x234568a0>>(t>>8&28))))|t/2>>(t>>11)^t>>12)+(t/16&t&24);
            break;
        case 59:// // bst 2011-10-05 http://pouet.net/topic.php?which=8357&page=12
			o=(t>>5)|(t>>4)|((t%42)*(t>>4)|(0x15483113)-(t>>4))/(t>>16)^(t|(t>>4));
            break;
        case 60:// // tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
			//o=(-t&4095)*(255&t*(t&(t>>13)))>>12)+(127&t*(234&t>>8&t>>3)>>(3&t>>14));
            break;
        case 61:// // ultrageranium 2011-10-12 http://0xa.kuri.mu/2011/10/09/bitop-videos/
			//o=(t*t/256)&(t>>((t/1024)%16))^t%64*(0xC0D3DE4D69>>(t>>9&30)&t%32)*t>>18;
            break;
        case 62:// // visy 2011-10-06 http://pouet.net/topic.php?which=8357&page=13
			o=(t%25-(t>>2|t*15|t%227)-t>>3)|((t>>5)&(t<<5)*1663|(t>>3)%1544)/(t%17|t%2048);
            break;
        case 63:// // ryg 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o 44.1 kHz
			o=((t*("36364689"[t>>13&7]&15))/12&128)+(((((t>>12)^(t>>12)-2)%11*t)/4|t>>13)&127);
            break;
        case 64:// // mu6k http://www.youtube.com/watch?v=tCRPUv8V22o 32.0 kHz
			//o=(3e3/(y=t&16383)&1)*35 +(x=t*"6689"[t>>16&3]/24&127)*y/4e4 +((t>>8^t>>10|t>>14|x)&63);
            break;
        case 65:// // Ola 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
			o=((1-(((t+10)>>((t>>9)&((t>>14))))&(t>>4&-2)))*2)*(((t>>10)^((t+((t>>6)&127))>>10))&1)*32+128;
            break;
        case 66:// // raer 2011-10-07 http://pouet.net/topic.php?which=8357&page=16 stereo 11kHz
            // L: ((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t)) R: t*(((t>>9)^((t>>9)-1)^1)%13)
            break;
        case 67:// // ryg 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
			o=((t>>4)*(13&(0x8898a989>>(t>>11&30)))&255)+((((t>>9|(t>>2)|t>>8)*10+4*((t>>2)&t>>15|t>>8))&255)>>1);
            break;
        case 68:// // gasman 2011-10-05 http://pouet.net/topic.php?which=8357&page=12 js-only
			//o=(t<<3)*[8/9,1,9/8,6/5,4/3,3/2,0][[0xd2d2c8,0xce4088,0xca32c8,0x8e4009][t>>14&3]>>(0x3dbe4688>>((t>>10&15)>9?18:t>>10&15)*3&7)*3&7];
            break;
        case 69: // a1k0n http://news.ycombinator.com/item?id=3063359 js-only
// SS=function(s,o,r,p){var c=s.charCodeAt((t>>r)%p);return c==32?0:31&t*Math.pow(2,c/12-o)},SS("0 0     7 7     037:<<",6,10,32) + (t&4096?SS("037",4,8,3)*(4096-(t&4095))>>12 : 0)
            break;
        case 70:
// // mu6k 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o "Long-line Theory", Chaos Theory cover, optimized by ryg, p01 et al., JS-only
// w=t>>9,k=32,m=2048,a=1-t/m%1,d=(14*t*t^t)%m*a,y=[3,3,4.7,2][p=w/k&3]*t/4,h="IQNNNN!!]]!Q!IW]WQNN??!!W]WQNNN?".charCodeAt(w/2&15|p/3<<4)/33*t-t,s=y*.98%80+y%80+(w>>7&&a*((5*t%m*a&128)*(0x53232323>>w/4&1)+(d&127)*(0xa444c444>>w/4&1)*1.5+(d*w&1)+(h%k+h*1.99%k+h*.49%k+h*.97%k-64)*(4-a-a))),s*s>>14?127:s
            break;
        default:
            o=i;
            break;
    }
    v=lowByte(o);
    PORTB = (PORTB & B11100000) | (v >> 3);
    PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);

    for(p=0;p<pause;p++); //slow it down

    if (clkState==HIGH){
        clkState=LOW;
        digitalWrite(3,HIGH);
        fn=map(analogRead(A2),0,1023,0,70);
        pause=analogRead(A0);
        i=0;
    }
    if (i>pulse){
        digitalWrite(3,LOW);
        i=0;
    }
}    
void isr(){
    clkState=HIGH;
}

//  ===================== end of program =======================
//
//
//
