
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

long interval = 10000;
long updateInterval = 5000;
int SensorPin = A0;
int PowerLed = 13;
int PumpPin = 7;
int relay = 9;
int sensorPOW = 8;
float temp = 0;
int percentage = 0;

bool on = true;

unsigned long prevTime = 0;
unsigned long prevUPTime = 0;

void setup() {


  lcd.begin(16, 2);
  
  pinMode(PumpPin, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(PowerLed, OUTPUT);
  pinMode(sensorPOW, OUTPUT);

  digitalWrite(relay, HIGH);
  digitalWrite(PowerLed, HIGH);

  //Pradines reiksmes
  getTempVal();

  getMoistureVal();

  printToLCD();
     
}

void loop() {

   getTempVal();
   
   unsigned long currTime = millis();
  
   if( (currTime - prevTime) > interval){
    prevTime = currTime;

   getMoistureVal();
   printToLCD();
   controlPump(percentage);
  
   }
  if( (currTime - prevUPTime) > updateInterval){
    prevUPTime = currTime;
    printToLCD();
  } 
}

int clamp(int value, int maxvalue){
  if(value <= maxvalue)
  return value;
  else if(value > maxvalue)
  return maxvalue;
}

void controlPump(int percentage){
  
  if(percentage < 50){
      digitalWrite(PumpPin, HIGH);
      digitalWrite(relay, LOW);
      delay(3000);
      digitalWrite(PumpPin, LOW);
      digitalWrite(relay, HIGH);
   }
}

void getMoistureVal(){
   digitalWrite(sensorPOW, HIGH);
   delay(2000);
  
   int val = analogRead(SensorPin);
   int realVal = 1023 - val;
   int clamped = clamp(realVal, 815);
   percentage = map(clamped, 0, 815, 0, 100); 

   digitalWrite(sensorPOW, LOW);
}

void getTempVal(){
   int sensorVal = analogRead(A1);
   float voltage = (sensorVal/1024.0) * 5.0;
   temp = (voltage - 0.5) * 100;
}

void printToLCD(){
   lcd.clear();
   lcd.print("dregme : ");
   lcd.print(percentage);
   lcd.print(" %");
   lcd.setCursor(0,1);
   lcd.print("temp. : ");
   lcd.print(temp);
   lcd.print(" C");
}
