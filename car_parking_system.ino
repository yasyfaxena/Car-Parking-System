#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

int IR1 = 4;
int IR2 = 3;
int ledMerah = 8;
int ledHijau = 9;
int Slot = 4;
int flag1 = 0;
int flag2 = 0;

unsigned long lastDisplayTime = 0;
bool showCarParking = true;

unsigned long debounceTime = 500;
unsigned long stableTimeIR1 = 0;
unsigned long stableTimeIR2 = 0;
bool IR1Stable = false;
bool IR2Stable = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(ledMerah, OUTPUT);
  pinMode(ledHijau, OUTPUT);

  myservo.attach(2);
  myservo.write(100);

  digitalWrite(ledMerah, HIGH);
  digitalWrite(ledHijau, LOW);
}

void loop() {
  unsigned long currentTime = millis();

  // Debounce sensor IR1 (pintu masuk)
  if (digitalRead(IR1) == LOW) {
    if (!IR1Stable) {
      if (currentTime - stableTimeIR1 > debounceTime) {
        IR1Stable = true;
      }
    }
  } else {
    IR1Stable = false;
    stableTimeIR1 = currentTime;
  }

  // Debounce sensor IR2 (pintu keluar)
  if (digitalRead(IR2) == LOW) {
    if (!IR2Stable) {
      if (currentTime - stableTimeIR2 > debounceTime) {
        IR2Stable = true;
      }
    }
  } else {
    IR2Stable = false;
    stableTimeIR2 = currentTime;
  }

  // ===== Kendaraan MASUK =====
  if (IR1Stable && flag1 == 0) {
    if (Slot > 0) {
      flag1 = 1;
      myservo.write(0);
      digitalWrite(ledMerah, LOW);
      digitalWrite(ledHijau, HIGH);
      Slot = Slot - 1;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   WELCOME!   ");
      lcd.setCursor(0, 1);
      lcd.print(" Slot Left : ");
      lcd.print(Slot);
      delay(3000);

      myservo.write(100);
      digitalWrite(ledMerah, HIGH);
      digitalWrite(ledHijau, LOW);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   SORRY!   ");
      lcd.setCursor(0, 1);
      lcd.print(" NO PARKING SLOTS ");
      delay(3000);
    }
  }

  // ===== Kendaraan KELUAR =====
  if (IR2Stable && flag2 == 0) {
    flag2 = 1;
    myservo.write(0);
    digitalWrite(ledMerah, LOW);
    digitalWrite(ledHijau, HIGH);
    Slot = Slot + 1;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   GOODBYE!   ");
    lcd.setCursor(0, 1);
    lcd.print("DRIVE CAREFULLY:>");
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" PARKING SLOTS ");
    lcd.setCursor(0, 1);
    lcd.print(" Slot Left : ");
    lcd.print(Slot);
    delay(3000);

    myservo.write(100);
    digitalWrite(ledMerah, HIGH);
    digitalWrite(ledHijau, LOW);
  }

  // Reset flag setelah salah satu proses selesai
  if (flag1 == 1 || flag2 == 1) {
    flag1 = 0;
    flag2 = 0;
  }

  // ===== Tampilan LCD bergantian (idle) =====
  if (millis() - lastDisplayTime > 2000) {
    lastDisplayTime = millis();
    lcd.clear();
    if (showCarParking) {
      lcd.setCursor(0, 0);
      lcd.print(" CAR PARKING ");
    } else {
      lcd.setCursor(0, 0);
      lcd.print(" PARKING SLOTS ");
      lcd.setCursor(0, 1);
      lcd.print(" Slot Left : ");
      lcd.print(Slot);
    }
    showCarParking = !showCarParking;
  }
}
