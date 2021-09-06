#include <EEPROM.h>  

#define aPin 2  //        
#define bPin 3  //      _____
#define cPin 4  //     |  A  |
#define dPin 5  //   F |_____| B
#define ePin 6  //     |  G  |
#define fPin 7  //   E |_____| C
#define gPin 8  //        D     O dot
#define dotPin 13

#define c1Pin 9  // Common pin for digit 1
#define c2Pin 10 // Common pin for digit 2
#define c3Pin 11 // Common pin for digit 3
#define c4Pin 12 // Common pin for digit 4

int Common = 1;   //<Common=0; for Common cathode> <Common=1; for Common anode>
int Set_mode = 1; //<Set_mode=0; for 00:00 HH:MM> <Set_mode=1; for 00:00 MM:SS>

#define bt_set  A0
#define bt_up   A1
#define bt_down A2

#define Relay  A4
#define buzzer A5

int hh=0, mm=0, ss=0;
word MilliSecond = 0; 
bool timerStart = false;

int flag1=0, timer=0, Set=0, Mode=1;

int On, Off, Off_C; 
int DTime = 4; // Display timer

void setup(){ // put your setup code here, to run once  

 pinMode(bt_set, INPUT_PULLUP); 
 pinMode(bt_up,    INPUT_PULLUP);
 pinMode(bt_down,  INPUT_PULLUP);

 pinMode(aPin, OUTPUT);
 pinMode(bPin, OUTPUT);
 pinMode(cPin, OUTPUT);
 pinMode(dPin, OUTPUT);
 pinMode(ePin, OUTPUT); 
 pinMode(fPin, OUTPUT);
 pinMode(gPin, OUTPUT);
 pinMode(dotPin, OUTPUT);

 pinMode(c1Pin, OUTPUT);
 pinMode(c2Pin, OUTPUT);
 pinMode(c3Pin, OUTPUT);
 pinMode(c4Pin, OUTPUT);

 pinMode(Relay, OUTPUT); digitalWrite(Relay, Mode);
 pinMode(buzzer, OUTPUT);

 if(Common==1){On=0, Off=1, Off_C=0;} //for Common anode
          else{On=1, Off=0, Off_C=1;} //for Common cathode

 // Indicate that system is ready
 for (int i = 9; i >=0; i--) {
 showNumber(i);
 digitalWrite(c1Pin, Common);
 digitalWrite(c2Pin, Common);
 digitalWrite(c3Pin, Common);
 digitalWrite(c4Pin, Common);
 delay(1000); 
 } 

 noInterrupts();         // disable all interrupts
 TCCR1A = 0;             // set entire TCCR1A register to 0  //set timer1 interrupt at 1kHz  // 1 ms
 TCCR1B = 0;             // same for TCCR1B
 TCNT1  = 0;             // set timer count for 1khz increments
 OCR1A = 1999;           // = (16*10^6) / (1000*8) - 1
 //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
 // turn on CTC mode
 TCCR1B |= (1 << WGM12); // Set CS11 bit for 8 prescaler
 TCCR1B |= (1 << CS11);  // enable timer compare interrupt
 TIMSK1 |= (1 << OCIE1A);
 interrupts();           // enable

//EEPROM.write(11, mm);
//EEPROM.write(12, hh); 

read_eeprom();
}

void loop(){  

if(digitalRead (bt_set) == 0){ 
if(flag1==0 && Mode==1){ flag1=1; timer = 30;
digitalWrite(buzzer, HIGH);
Set = Set+1;
if(Set>2){Set=0;} 
read_eeprom(); 
 }
}else{flag1=0;}

if(digitalRead (bt_up) == 0 && timer==0){ timer = 20;
digitalWrite(buzzer, HIGH);
if(Set==0){Mode = !Mode;
if(hh>0 || mm>0){
if(Mode==0){timerStart = true;}
}else{Mode=1;}

if(Mode==1){timerStart = false;}
}

if(Set==1){mm = mm+1;
  if(mm>59)mm=0;
EEPROM.write(11, mm);
}

if(Set==2){hh = hh+1;
  if(hh>99)hh=0;
EEPROM.write(12, hh);
}
}

if(digitalRead (bt_down) == 0 && timer==0){ timer = 20;
digitalWrite(buzzer, HIGH);
if(Set==0){read_eeprom(); timerStart = true; Mode=0;}

if(Set==1){mm = mm-1;
if(mm<0)mm=59;
EEPROM.write(11, mm);
}

if(Set==2){hh = hh-1;
if(hh<0)hh=99;
EEPROM.write(12, hh);
}

}

if(hh==0 && mm==0 && Mode==0){Mode=1; timerStart = false;
timer = 50;
digitalWrite(buzzer, HIGH);
}  

if(Set==1){all_off();}
else{showNumber((hh/10)%10);}
digitalWrite(c1Pin, Common);
delay(DTime);
digitalWrite(c1Pin, Off_C);

if(Set==1){all_off();}
else{showNumber(hh%10);}
digitalWrite(c2Pin, Common);
delay(DTime);
digitalWrite(c2Pin, Off_C);

if(Set==2){all_off();}
else{showNumber((mm/10)%10);
if(MilliSecond<500 && Mode==0){digitalWrite( dotPin, Off);}
else{digitalWrite( dotPin, On);}
}
digitalWrite(c3Pin, Common);
delay(DTime);
digitalWrite(c3Pin, Off_C);

if(Set==2){all_off();}
else{showNumber(mm%10); }
digitalWrite(c4Pin, Common);
delay(DTime);
digitalWrite(c4Pin, Off_C);
digitalWrite( dotPin, Off);

digitalWrite(Relay, Mode);


if(timer>0){timer=timer-1;}
if(timer==0){digitalWrite(buzzer, LOW);}
}

void read_eeprom(){
mm = EEPROM.read(11);
hh = EEPROM.read(12); 
}

void showNumber(int x){
  
   switch(x){
     case 1: one();   break;
     case 2: two();   break;
     case 3: three(); break;
     case 4: four();  break;
     case 5: five();  break;
     case 6: six();   break;
     case 7: seven(); break;
     case 8: eight(); break;
     case 9: nine();  break;
     default: zero(); break;
   }
}

void all_off(){
  digitalWrite( aPin, Off); //      
  digitalWrite( bPin, Off); //      
  digitalWrite( cPin, Off); //      
  digitalWrite( dPin, Off); //      
  digitalWrite( ePin, Off); //      
  digitalWrite( fPin, Off);
  digitalWrite( gPin, Off);
  digitalWrite( dotPin, Off);
}

void one(){
  digitalWrite( aPin, Off); //      
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //      |
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, Off);
  digitalWrite( gPin, Off);
}

void two(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      | 
  digitalWrite( cPin, Off); //  ____|
  digitalWrite( dPin, On);  // |     
  digitalWrite( ePin, On);  // |____
  digitalWrite( fPin, Off);
  digitalWrite( gPin, On);
}

void three(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //  ____|
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, Off); 
  digitalWrite( gPin, On);
}

void four(){
  digitalWrite( aPin, Off); //  
  digitalWrite( bPin, On);  // |    |
  digitalWrite( cPin, On);  // |____|
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void five(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, Off); // |
  digitalWrite( cPin, On);  // |____
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, On);  
  digitalWrite( gPin, On);
}

void six(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, Off); // |
  digitalWrite( cPin, On);  // |____
  digitalWrite( dPin, On);  // |    |
  digitalWrite( ePin, On);  // |____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void seven(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //      |
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, Off);
  digitalWrite( gPin, Off);
}

void eight(){
  digitalWrite( aPin, On); //  ____
  digitalWrite( bPin, On); // |    |
  digitalWrite( cPin, On); // |____|
  digitalWrite( dPin, On); // |    |
  digitalWrite( ePin, On); // |____|
  digitalWrite( fPin, On); 
  digitalWrite( gPin, On); 
}

void nine(){
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  // |    |
  digitalWrite( cPin, On);  // |____|
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, On); 
  digitalWrite( gPin, On);
}

void zero(){                 
  digitalWrite( aPin, On); //  ____
  digitalWrite( bPin, On); // |    |
  digitalWrite( cPin, On); // |    |
  digitalWrite( dPin, On); // |    |
  digitalWrite( ePin, On); // |____|
  digitalWrite( fPin, On); 
  digitalWrite( gPin, Off);   
}


ISR(TIMER1_COMPA_vect){   
if(timerStart == true){MilliSecond++;
    if(MilliSecond >= 1000){MilliSecond = 0;
       if(Set_mode==0){ss--;
       if(ss<0){ss=59; mm--;}
       }else{mm--;}
       if(mm<0){mm=59; hh--;}
    }
  }  
}
