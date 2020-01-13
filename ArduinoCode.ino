#include <dht.h>
#include <LiquidCrystal.h>
#include <AutoPID.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

dht DHT;
//Use PIN 9 and 6, those have PWM option....

int Hum_fan = 6;
int Temp_Fan = 9; 
int Fan_Speed = 0; 

int Heater;
//int HeaterSpeed;
//This Will Be for the PID SYSTEM!
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
/*
 * How we set this up is -> start with KP.... Make it have Swining Behav 
 * Adjust KI to kill the error.... 
 * Adjust KD to make it react faster 
 */
#define KP 10
#define KI 0.4
#define KD 0

double HumRead, setPoint, outputHum;
double TempRead,SetPoint,OutPutTemp;

AutoPID myPID(&HumRead, &setPoint, &outputHum, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
// from 100 - 255 PWM.... Needs that for temp.... 
AutoPID my2PID(&TempRead, &SetPoint, &OutPutTemp, OUTPUT_MIN,OUTPUT_MAX,KP,KI,KD);

#define DHT11_PIN 7

void setup() {
  lcd.begin(16, 2);
  pinMode(Temp_Fan,OUTPUT);
  pinMode(Hum_fan, OUTPUT);
  pinMode(Heater, OUTPUT);
  setPwmFrequency(9, 6);
  Serial.begin(9600); 
  Fan_Speed = 0; 
  SetPoint = 50; 
}


void loop()
{
  int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0, 0);
  
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
  
  Lower_HUM_FAN();
  PID_TEMP_FAN();
  ShowValues(); // This Will Send Current Data/Values...
  //HeaterControll();
  delay(1250);
}
/*
 * Heater Should Reach Temp Wanted
 * And Then keep it at that point... PID system Here aswell? 
 */
void HeaterControll() {

  //HeaterSpeed = (12/12)*256;
  
  //digitalWrite(Heater,HIGH); 

}
void PID_TEMP_FAN(){
  int out = 0;
  int offset = 110; 
  TempRead = DHT.humidity;
  my2PID.run();
  out = 255 - OutPutTemp + offset; 
  if (out > 255)
  {
    out = 255; 
  }
  if (out == offset){
    out = 0; 
  }
  analogWrite(Temp_Fan, out);

}
// PID system for HUM fan....... 
// while(Hum < 30) KEEP PID on 
// End with Giving HUM_FAN LOW
void Lower_HUM_FAN() {

  //setPoint = 35;
  //HumRead = DHT.humidity;
  //myPID.run();
  //analogWrite(Hum_fan, outputHum); 
  digitalWrite(Hum_fan,HIGH);
  
  
}


void ShowValues() {
  int value = DHT.humidity;
  int out = 0;
  int offset = 110; 
  Serial.print((value)); //Send that the Averge Temp for 8 ticks... or 1250 * 8 =
  Serial.print("\n");
  value = DHT.temperature;
  
  out = 255 - OutPutTemp + offset; 
  
  Serial.print((out));
  Serial.print("\n");  
 
}
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
