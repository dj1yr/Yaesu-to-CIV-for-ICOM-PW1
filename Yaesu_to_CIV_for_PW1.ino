

int y_a = 2;
int y_b = 3;
int y_c = 4;
int y_d = 5;
int tx_in = 6;
int tx_out = 7;

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
boolean band_data = false;
boolean cmd_mode = false;
boolean tx_ok = false;
boolean start_loop =true;


void setup(){
Serial1.begin(9600);



pinMode(y_a,INPUT);
pinMode(y_b,INPUT);  
pinMode(y_c,INPUT);
pinMode(y_d,INPUT);
pinMode(LED_BUILTIN, OUTPUT);
pinMode(tx_in, INPUT_PULLUP);
pinMode(tx_out,OUTPUT);

}  

byte daten_pw1; 
int i;
int j;
int incomingTRX;                       //incoming character on TRX side
int incomingPC;                        //incoming character on PC side

int buffget_pw1tx[63];               // the receive buffer on TRX end               // the frequency bytes memory
boolean uart_busy;

void loop(){

y_a = digitalRead(2);
y_b = digitalRead(3);
y_c = digitalRead(4);
y_d = digitalRead(5);
tx_in = digitalRead(6);

// Yaesu bcd to CIV frequency ->Mhz+100*khz

if (y_a==0 && y_b==0 && y_c==0 && y_d==0){bnd_2=0x51; bnd_3=0x00; band=1;}    //no band signals -> select 6m send 51.00Mhz
if (y_a==1 && y_b==0 && y_c==0 && y_d==0){bnd_2=0x01; bnd_3=0x90; band=2;}    //160m send 1.90Mhz
if (y_a==0 && y_b==1 && y_c==0 && y_d==0){bnd_2=0x03; bnd_3=0x70; band=3;}    //80m send 3.70Mhz
if (y_a==1 && y_b==1 && y_c==0 && y_d==0){bnd_2=0x07; bnd_3=0x10; band=4;}    //60m/40m send 7.10Mhz
if (y_a==0 && y_b==0 && y_c==1 && y_d==0){bnd_2=0x10; bnd_3=0x05; band=5;}    //30m send 10.05Mhz
if (y_a==1 && y_b==0 && y_c==1 && y_d==0){bnd_2=0x14; bnd_3=0x20; band=6;}    //20m send 14.20Mhz
if (y_a==0 && y_b==1 && y_c==1 && y_d==0){bnd_2=0x18; bnd_3=0x06; band=7;}    //17m send 18.06Mhz
if (y_a==1 && y_b==1 && y_c==1 && y_d==0){bnd_2=0x21; bnd_3=0x20; band=8;}    //15m send 21.20Mhz
if (y_a==0 && y_b==0 && y_c==0 && y_d==1){bnd_2=0x24; bnd_3=0x90; band=9;}    //12m send 24.90Mhz
if (y_a==1 && y_b==0 && y_c==0 && y_d==1){bnd_2=0x28; bnd_3=0x50; band=10;}    //10m send 25.500Mhz
if (y_a==0 && y_b==1 && y_c==0 && y_d==1){bnd_2=0x51; bnd_3=0x00; band=11;}    //6m send 51.00Mhz
if (y_a==1 && y_b==1 && y_c==1 && y_d==1){bnd_2=0x51; bnd_3=0x00; band=12;}    // Yaesu 4m band selected, send 51.00Mhz



    if(uart_busy == false){
      if (band_old!=band){
      data_send_1();
      }
    }

    if (uart_busy==false){
      uart_busy = true;
      daten_pw1 = Serial1.read();

        if ((daten_pw1 == 254)){ 
            PA_data_start();
        }
      delay (1);  // allow buffer to fill
      uart_busy = false;

    }

    if(tx_in == LOW){
      uart_busy = true;
      data_send_1();
      digitalWrite(tx_out,HIGH);

    }
    else{
      digitalWrite(tx_out,LOW);
      uart_busy = false;
    }

    
}//*********end of loop

//-------------------------------------------------------------------
 void PA_data_start(){                                  // 1st byte was a 254
  //-------------------------------------------------------------------

delay(10);
  for ( i = 0; i < 6; i++) {                     

    if (Serial1.available() > 0) {           

      buffget_pw1tx[i] = Serial1.read();     

      String pw1_string = String(buffget_pw1tx[i], HEX);

     if (buffget_pw1tx[i] ==  253   ) {
      if (start_loop == false){
        data_send_3();
      }
      else{
        data_send_2();
      }
      }                          
    }
  }

 }
 void data_send_1(){
   uart_busy = true;
        Serial1.write(data_start,2);    //0xFE start of telegram, send 2x 0xFE
        Serial1.write(to_addr);      //PW1 adress
        Serial1.write(from_addr);    //TRX adress
        Serial1.write(cmd_1);    //10hz+1hz
        Serial1.write(bnd_5);    //1khz+100hz    
        Serial1.write(bnd_4);    //10khz
        Serial1.write(bnd_3);    //100khz
        Serial1.write(bnd_2);    //10Mhz
        Serial1.write(bnd_1);    //100Mhz
        Serial1.write(data_stop);    //0xFD end of telegramm
  band_old=band;
  uart_busy = false;
 }

  void data_send_2(){
  uart_busy = true;
        Serial1.write(data_start,2);    //0xFE start of telegram, send 2x 0xFE
        Serial1.write(to_addr);      //PW1 adress
        Serial1.write(from_addr);    //TRX adress
        Serial1.write(cmd_2);    
        Serial1.write(bnd_5);    //1khz+100hz    
        Serial1.write(bnd_4);    //10khz
        Serial1.write(bnd_3);    //100khz
        Serial1.write(bnd_2);    //10Mhz
        Serial1.write(bnd_1);    //100Mhz
        Serial1.write(data_stop);    //0xFD end of telegramm
        start_loop = false;
        uart_busy = false;
  }

 
  void data_send_3(){
  uart_busy = true;
        Serial1.write(data_start,2);    //0xFE start of telegram, send 2x 0xFE
        Serial1.write(to_addr);      //PW1 adress
        Serial1.write(from_addr);    //TRX adress
        Serial1.write(cmd_3);    //10hz+1hz
        Serial1.write(0x00);
        Serial1.write(mode);
        Serial1.write(data_stop);    //0xFD end of telegramm
  uart_busy = false;
 }
 
