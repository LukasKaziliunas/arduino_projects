#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int in = 8;
int led = 5;

int lightSensor = A0;

const int SAMPLE_TIME = 10;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
const int soundThreshold = 40;
int acl; //avarage amount of light 
int lightThreshold;

unsigned long millisLightning;
unsigned long millisThunder;
unsigned long millisDiff;
float distance = 0;
bool waiting;
bool noFlash = true;  // so only 1 lighting flash would count

int lightVal = 0;

int SampleBufferValue = 0;


void setup() {

  lcd.begin();
  lcd.backlight();
  pinMode(in, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(9600);

 delay(2000);
 acl = AvarageCurrentLight();
 lightThreshold = calculateSensitivity(acl);
 lcd.print("waiting for");
 lcd.setCursor(0, 1);
 lcd.print("lightning");
 // Serial.println(acl);
  // Serial.println(lightThreshold);
}

void loop() {
    
    lightVal = analogRead(lightSensor);
     
    if(lightVal > lightThreshold && noFlash) 
    {
      lcd.clear();
      lcd.print("waiting for");
      lcd.setCursor(0, 1);
      lcd.print("thunder");
      noFlash = false;
      millisLightning = millis();
      digitalWrite(led, HIGH);
      waiting = true;

      while(waiting)
      {
        millisCurrent = millis();
        millisElapsed = millisCurrent - millisLast;

        if(digitalRead(in) == LOW){
        SampleBufferValue++;
        }

        if(SampleBufferValue > soundThreshold){
        digitalWrite(led, LOW);
        waiting = false;
        millisThunder = millis();
        }

        if(millisElapsed > SAMPLE_TIME){
        //  Serial.println(SampleBufferValue);
        SampleBufferValue = 0;
        //digitalWrite(led, LOW);
        millisLast = millisCurrent;
        }
      }

      millisDiff = millisThunder - millisLightning;
      //Serial.println(millisDiff);
      distance = (millisDiff / 1000.00) / 3 ;
      //Serial.println(distance);
      lcd.clear();
      lcd.print(distance);
      lcd.print(" km");
    }  
}

int AvarageCurrentLight(){
  long sum = 0;
  int avg = 0;
  lcd.print("calibrating");

  for(int i = 0; i < 1000; i++)
  {
    int val = analogRead(lightSensor);
        sum = sum + val;
        delay(3);
  }

  avg = sum / 1000;  
  lcd.clear();
  return avg;
}

int calculateSensitivity(int light)
{
   double keof = map(light, 1, 1000, 1.25, 1);
   return light * keof;
}
