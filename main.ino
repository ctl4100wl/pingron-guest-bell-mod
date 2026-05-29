#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 21);

const int PIR_PIN = 14; 
const int BELL_TRIGGER_PIN = 12; // Pin connected to SZY42F Pin 2

unsigned long lastTriggerTime = 0;
const unsigned long cooldownDelay = 1000; // 1-second delay so it doesn't spam the chime

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(BELL_TRIGGER_PIN, OUTPUT);
  
  // Keep the bell silent at startup
  digitalWrite(BELL_TRIGGER_PIN, LOW); 

  u8g2.begin();
  u8g2.enableUTF8Print(); 

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 25);
  u8g2.print("Calibrating PIR...");
  u8g2.setCursor(0, 45);
  u8g2.print("Stay still for 15s...");
  u8g2.sendBuffer();
  
  delay(15000); 
}

void loop() {
  int motionStatus = digitalRead(PIR_PIN);
  unsigned long currentTime = millis();

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 12);
  u8g2.print("PIR check");
  u8g2.drawHLine(0, 16, 128);

  if (motionStatus == HIGH) {
    u8g2.setFont(u8g2_font_9x15_tf); 
    u8g2.setCursor(5, 48);
    u8g2.print("INTRUDER!"); 
    Serial.println("Motion detected!");

    // Trigger the PINGRON bell if cooldown has passed
    if (currentTime - lastTriggerTime >= cooldownDelay) {
      Serial.println("Sending trigger to PINGRON MCU...");
      digitalWrite(BELL_TRIGGER_PIN, HIGH);
      delay(400); // 400ms pulse to simulate the trigger
      digitalWrite(BELL_TRIGGER_PIN, LOW);
      
      lastTriggerTime = currentTime; // Reset cooldown clock
    }
  } else {
    u8g2.setFont(u8g2_font_9x15_tf);
    u8g2.setCursor(5, 48);
    u8g2.print("SECURE");
    Serial.println("Scanning... No motion.");
  }

  u8g2.sendBuffer(); 
  delay(200); 
}
