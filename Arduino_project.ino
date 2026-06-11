#include <Servo.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 8, 12, 13);

Servo base_servo;
Servo arm_servo;

// RGB LED Pins
int redPin = 9;
int greenPin = 10;
int bluePin = 11;

// LDR Pin
int ldrPin = A2;

// Infrared pin
int irpin=A0;

// Color values
int ldrRed = 0, ldrGreen = 0, ldrBlue = 0;

// Servo positions
int arm_pos = 100;
int base_pos = 0;

// Timing
int delayBase = 1000;

// ir value
int irvalue;

// Color names (set these via Serial before starting)
String pickupColor = "";
String dropoffColor = "";

// --------------------- SETUP ---------------------
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(irpin,INPUT);
  pinMode(7,OUTPUT);

  base_servo.attach(5);
  arm_servo.attach(6);

  lcd.print("Color Sorter Bot");
  delay(1000);
  lcd.clear();

  Serial.println("Enter PICKUP color (red / green / blue): ");
  while (pickupColor == "") {
    if (Serial.available()) {
      pickupColor = Serial.readStringUntil('\n');
      pickupColor.trim();
      pickupColor.toLowerCase();
    }
  }

  Serial.println("Enter DROPOFF color (red / green / blue): ");
  while (dropoffColor == "") {
    if (Serial.available()) {
      dropoffColor = Serial.readStringUntil('\n');
      dropoffColor.trim();
      dropoffColor.toLowerCase();
    }
  }

  lcd.clear();
  lcd.print("Pickup: ");
  lcd.print(pickupColor);
  lcd.setCursor(0, 1);
  lcd.print("Drop: ");
  lcd.print(dropoffColor);
  delay(2000);
  lcd.clear();

  Serial.println("Setup Complete!");
  Serial.print("Pickup Color: "); Serial.println(pickupColor);
  Serial.print("Dropoff Color: "); Serial.println(dropoffColor);
  Serial.println("Searching for pickup color...");
}

// --------------------- MAIN LOOP ---------------------
void loop() {
  // detectedColor needs to be visible across steps
  String detectedColor = "";

  // Rotate base looking for pickup color
  while (base_pos != 120) {
    base_servo.write(base_pos);
    base_pos++;
    delay(100);

    auto_calibration();
    detectedColor = detectColor();
    Serial.print("Color is: ");
    Serial.println(detectedColor);

    if (detectedColor == pickupColor) {
      Serial.println("Pickup color found - stopping base.");
      base_servo.detach();
      break; // go to pickup step
    }
  }

  //PICKUP: moving the arm down for picking the object
  while (1)
  {
  irvalue=analogRead(A0);
  arm_servo.write(arm_pos);
  arm_pos++;
  delay(50);
  if (irvalue<50)
  {
    digitalWrite(7,1);
    break;
  }
  }
  
  delay(2000);

  // moving the arm up along with the object
  while (arm_pos!=100){
  irvalue=analogRead(A0);
  arm_servo.write(arm_pos);
  arm_pos--;
  delay(50);
  }

  // Re-attach base and search for dropoff color
  base_servo.attach(5);
  base_pos = 0; // start scanning from a safe home
  while (base_pos != 130) {
    base_servo.write(base_pos);
    base_pos++;
    delay(50);

    auto_calibration();
    detectedColor = detectColor();
    Serial.print("Detected while scanning for dropoff: ");
    Serial.println(detectedColor);
    
    if (detectedColor == dropoffColor) {
      Serial.println("Dropoff color found - stopping base.");
      base_servo.detach();
      break; // go to drop step
    }
  }

  // ----- DROPOFF: move arm down to release then up -----
  while (1)
  {
  irvalue=analogRead(A0);
  arm_servo.write(arm_pos);
  arm_pos++;
  delay(50);
  if (irvalue<50)
  {
    digitalWrite(7,0);
    break;
  }
  }
  
  delay(2000);

  while (arm_pos!=100)
  {
    irvalue=analogRead(A0);
    arm_servo.write(arm_pos);
    arm_pos--;
    delay(50);
  }

  base_servo.attach(5);
  base_pos=0;

  // Re-attach base and return to a safe home position
 
  

  // Small pause before next cycle (or you can wait for new Serial commands)
  
}

// --------------------- COLOR DETECTION ---------------------
void auto_calibration() {
  // RED
  digitalWrite(redPin, HIGH);
  delay(30);
  ldrRed = analogRead(ldrPin);
  digitalWrite(redPin, LOW);

  // GREEN
  digitalWrite(greenPin, HIGH);
  delay(30);
  ldrGreen = analogRead(ldrPin);
  digitalWrite(greenPin, LOW);

  // BLUE
  digitalWrite(bluePin, HIGH);
  delay(40);
  ldrBlue = analogRead(ldrPin);
  digitalWrite(bluePin, LOW);
}

String detectColor() {
  // smaller value means more reflected light (brighter)
  if (ldrRed < ldrGreen && ldrRed < ldrBlue) return "red";
  else if (ldrGreen < ldrRed && ldrGreen < ldrBlue) return "green";
  else if (ldrBlue < ldrRed && ldrBlue < ldrGreen) return "blue";
  else return "unknown";
}
