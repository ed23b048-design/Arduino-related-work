#include <LiquidCrystal.h>

LiquidCrystal lcd(11,12,7,6,5,4);

int threshold=600;

void setup(){

  pinMode(3,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(A3,INPUT);
  Serial.begin(9600);
  lcd.begin(16,2);
  
}

void loop()
{
  
  int val=analogRead(A3);
  int pack=0;
  
  
  if(pack==0)
 {
    Serial.println(val);
    lcd.print(val);
    delay(100);
    lcd.clear();
    
  
  
  if (val>threshold){
    digitalWrite(8,1);
    digitalWrite(3,1);
    
    lcd.setCursor(0,0);
    lcd.print("Threshold");
    lcd.setCursor(1,1);
    lcd.print("Crossed ");
    pack=1;
    delay(1000);
    lcd.clear();
    delay(100);
  }
    
  else{
    digitalWrite(8,0);
    digitalWrite(3,0);
  }  
  
 }

}