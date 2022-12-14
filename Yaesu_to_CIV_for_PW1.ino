//Atmega8 config
#define uart Serial
int y_a = 17;
int y_b = 16;
int y_c = 15;
int y_d = 14;
int tx_in = 18;
int tx_out = 19;
int led = 13;             // led for debug



/*// Arduino Mega 2560 config
#define uart Serial1
int y_a = 2;
int y_b = 3;
int y_c = 4;
int y_d = 5;
int tx_in = 6;
int tx_out = 7;
*/


byte band=0;              //actual band from Yaesu TRX
byte band_old=0;
byte data_start[2]={0xFE,0xFE};
  
byte to_addr = 0x54;      // PW1 adress
byte from_addr = 0x30;    //TRX adress, e.g. IC-726 
byte cmd_1 = 0x00;
byte cmd_2 = 0x03;    //frequ. request
byte cmd_3 = 0x04;    //mode request

byte bnd_1 = 0x00;    //100*Mhz
byte bnd_2 = 0x51;    //10*Mhz
byte bnd_3 = 0x00;    //100*khz
byte bnd_4 = 0x00;    //10*khz
byte bnd_5 = 0x00;    //1*khz +100*hz

byte mode = 0x02;     //mode = lsb
byte data_stop = 0xFD;

byte cmd_mode = 0;    //cmd_mode = 0 -> 0x00, 1 -> 0x03, 2 -> 0x04

boolean tx_ok = false;

void setup(){
uart.begin(9600);

pinMode(y_a,INPUT);
pinMode(y_b,INPUT);  
pinMode(y_c,INPUT);
pinMode(y_d,INPUT);
//pinMode(led, OUTPUT);
pinMode(tx_in, INPUT_PULLUP);
pinMode(tx_out,OUTPUT);

}//end of setup  

byte daten_pw1; 
int i;

int buffer_pw1tx[6];               //receive buffer, 6 bytes from PW1 

void loop(){

/*
// Arduino Mega 2560
y_a = digitalRead(2);
y_b = digitalRead(3);
y_c = digitalRead(4);
y_d = digitalRead(5);
tx_in = digitalRead(6);
*/

// Atmega 8
y_a = digitalRead(17);
y_b = digitalRead(16);
y_c = digitalRead(15);
y_d = digitalRead(14);
tx_in = digitalRead(18);


// Yaesu bcd to CIV frequency ->Mhz+100*khz

if (y_a==0 && y_b==0 && y_c==0 && y_d==0){bnd_2=0x51; bnd_3=0x00; tx_ok = false; band=1;}     //no band signals -> select 6m send 51.00Mhz, TX disabled
if (y_a==1 && y_b==0 && y_c==0 && y_d==0){bnd_2=0x01; bnd_3=0x90; tx_ok = true; band=2;}      //160m send 1.90Mhz, TX enabled
if (y_a==0 && y_b==1 && y_c==0 && y_d==0){bnd_2=0x03; bnd_3=0x70; tx_ok = true; band=3;}      //80m send 3.70Mhz, TX enabled
if (y_a==1 && y_b==1 && y_c==0 && y_d==0){bnd_2=0x07; bnd_3=0x10; tx_ok = true; band=4;}      //60m/40m send 7.10Mhz, TX enabled
if (y_a==0 && y_b==0 && y_c==1 && y_d==0){bnd_2=0x10; bnd_3=0x05; tx_ok = true; band=5;}      //30m send 10.05Mhz, TX enabled
if (y_a==1 && y_b==0 && y_c==1 && y_d==0){bnd_2=0x14; bnd_3=0x20; tx_ok = true; band=6;}      //20m send 14.20Mhz, TX enabled
if (y_a==0 && y_b==1 && y_c==1 && y_d==0){bnd_2=0x18; bnd_3=0x06; tx_ok = true; band=7;}      //17m send 18.06Mhz, TX enabled
if (y_a==1 && y_b==1 && y_c==1 && y_d==0){bnd_2=0x21; bnd_3=0x20; tx_ok = true; band=8;}      //15m send 21.20Mhz, TX enabled
if (y_a==0 && y_b==0 && y_c==0 && y_d==1){bnd_2=0x24; bnd_3=0x90; tx_ok = true; band=9;}      //12m send 24.90Mhz, TX enabled
if (y_a==1 && y_b==0 && y_c==0 && y_d==1){bnd_2=0x28; bnd_3=0x50; tx_ok = true; band=10;}     //10m send 25.500Mhz, TX enabled
if (y_a==0 && y_b==1 && y_c==0 && y_d==1){bnd_2=0x51; bnd_3=0x00; tx_ok = true; band=11;}     //6m send 51.00Mhz, TX enabled
if (y_a==1 && y_b==1 && y_c==1 && y_d==1){bnd_2=0x51; bnd_3=0x00; tx_ok = false; band=12;}    // Yaesu 4m band selected, send 51.00Mhz, TX disabled


      if (band_old!=band){
      cmd_mode=0;  
      data_send();
      }
    
      daten_pw1 = uart.read();
      if ((daten_pw1 == 254)){ 
        PA_data_start();
        }
        delay (1);                      // time for buffer to fill
    
      if(tx_in == LOW && tx_ok == true){
      digitalWrite(tx_out,HIGH);
      }
      else{
      digitalWrite(tx_out,LOW);
      }

    
}//end of loop


void PA_data_start(){                                  

  delay(10);
  
    for ( i = 0; i < 6; i++) {                     
      if (uart.available() > 0) {           
        buffer_pw1tx[i] = uart.read();                           
      }
    }
      if ((buffer_pw1tx[1] == from_addr) && ( buffer_pw1tx[2] ==  to_addr)&&(buffer_pw1tx[3] == cmd_2)){    //check wich command is send from PW1 -> frequence request
          cmd_mode=1;        
          data_send();
      }

      if ((buffer_pw1tx[1] == from_addr) && ( buffer_pw1tx[2] ==  to_addr)&&(buffer_pw1tx[3] == cmd_3)){    //check wich command is send from PW1 -> mode request
          cmd_mode=2;        
          data_send();
      }
}// end of void PA_data_start
  
  void data_send(){
        uart.write(data_start,2); //0xFE start of telegram, send 2x 0xFE
         
        if(cmd_mode == 0){
        uart.write(0x00);
        uart.write(from_addr);    //TRX adress   
        //uart.write(cmd_1);     
        uart.write(cmd_2); 
        uart.write(bnd_5);        //1khz+100hz    
        uart.write(bnd_4);        //10khz
        uart.write(bnd_3);        //100khz
        uart.write(bnd_2);        //10Mhz
        uart.write(bnd_1);        //100Mhz                 
        }
        else if (cmd_mode == 1){
        uart.write(to_addr);      //PW1 adress
        uart.write(from_addr);    //TRX adress                  
        uart.write(cmd_2);
        uart.write(bnd_5);        //1khz+100hz    
        uart.write(bnd_4);        //10khz
        uart.write(bnd_3);        //100khz
        uart.write(bnd_2);        //10Mhz
        uart.write(bnd_1);        //100Mhz        
        }
        else{
        uart.write(to_addr);      //PW1 adress
        uart.write(from_addr);    //TRX adress                  
        uart.write(cmd_3);
        uart.write(0x00);
        uart.write(mode);         //mode = lsb        
        }
        uart.write(data_stop);    //0xFD end of telegramm
        band_old=band;    
  }
