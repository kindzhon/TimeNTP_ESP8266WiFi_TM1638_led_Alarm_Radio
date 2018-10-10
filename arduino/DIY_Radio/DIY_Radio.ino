#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#define clk 2
#define dt 3
#define sw 4
#define cs A0
LiquidCrystal lcd(5,6,7,8,9,10);
double frequency;
unsigned char frequencyH = 0;
unsigned char frequencyL = 0;
unsigned int frequencyB;
int Vol;
boolean mode;

volatile boolean TurnDetected;
volatile boolean up;

byte customChar[8] = {  //creates the arrow 
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11110,
  0b11100,
  0b11000,
  0b10000
};

void isr0 ()  {
  TurnDetected = true;
  up = (digitalRead(clk) == digitalRead(dt));
}

void setFrequency()  {
  frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  frequencyH = frequencyB >> 8;
  frequencyL = frequencyB & 0XFF;
  Wire.beginTransmission(0x60);
  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write((byte)0x00);
  Wire.endTransmission(); 
} 

void setVolume() {
 digitalWrite(cs, LOW);
 SPI.transfer(0);
 SPI.transfer(Vol);
 digitalWrite(cs, HIGH);
}

void displaydata(){
 lcd.setCursor(1,0);
 lcd.print("FM:");
 lcd.print(frequency);
 lcd.setCursor(1,1);
 lcd.print("Vol:");
 lcd.print(100-Vol*100/255);
 lcd.print("%");
} 

void arrow(){
 lcd.begin(16, 2);
 if (mode == 1){
   lcd.setCursor(0,0);
   lcd.write((uint8_t)0);}
 else{
   lcd.setCursor(0,1);
   lcd.write((uint8_t)0);} 
}

void setup() {
  SPI.begin();
  Wire.begin();
  lcd.begin(16, 2);
  pinMode(cs,OUTPUT);
  pinMode(clk,INPUT);
  pinMode(dt,INPUT);  
  pinMode(sw,INPUT);
  mode = 1; //frequency mode
  lcd.createChar(0, customChar); // arrow Char created
  frequency = 99.8; //starting Frequency
  Vol = 220; //starting Volume
  setFrequency();
  setVolume();
  arrow();
  displaydata();
  attachInterrupt (0,isr0,FALLING);
}

void loop() {
  if (!(digitalRead(sw))){
    mode=!mode;
    arrow();
    displaydata();
    delay(500);}
    
  if (TurnDetected && mode == 1){
    if(up){
      if (frequency >= 107.90){
        setFrequency();
        arrow();
        displaydata();}
      else{   
      frequency = frequency + 0.1;
      setFrequency();
      arrow();
      displaydata();}}
    else{
      if (frequency <= 87.6){
        setFrequency();
        arrow();
        displaydata();}
      else{
      frequency = frequency - 0.1;
      setFrequency();
      arrow();
      displaydata();}}
      TurnDetected = false;
    }

  if (TurnDetected && mode == 0){
    if(up){
      Vol = Vol - 10;
      if (Vol <= 0){
        Vol = 0;
        setVolume();
        arrow();
        displaydata();}
      else{
      setVolume();
      arrow();
      displaydata();}}
    else{
      Vol = Vol + 10;
      if (Vol >= 255){
        Vol = 255;
        setVolume();
        arrow();
        displaydata();}
      else{
      setVolume();
      arrow();
      displaydata();}}
      TurnDetected = false;
    }   
  }


    
  
 


