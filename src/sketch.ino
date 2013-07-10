#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal_SR.h>
#include <RTClib.h>

#define BUTTON_1	6
#define BUTTON_2	7
#define BUTTON_3	8
#define BUTTON_4	9

#define RTC_SDA		2
#define RTC_SCL		3

#define HEAT		3
#define COLD		4

#define TEMP_SENSOR	2

#define DEBOUNCE_DELAY	50

#define CLOCK		10
#define DATA		11
#define LATCH		12

#define READ_INTERVAL	500

long lastTempRead;

void printTime();

typedef struct {
   int pin;
   int state;
   long lastDebounceTime;
   int lastState;
   boolean pushed;
} button;

button buttons[4];

LiquidCrystal_SR lcd(DATA, CLOCK, LATCH);

OneWire oneWire(TEMP_SENSOR);
DallasTemperature sensors(&oneWire);

RTC_DS1307 RTC;

void setup(){
   Serial.begin(9600);

   Wire.begin();
   RTC.begin();

   if (! RTC.isrunning()) {
      RTC.adjust(DateTime(__DATE__, __TIME__));
   }

   buttons[0].pin = BUTTON_1;
   buttons[1].pin = BUTTON_2;
   buttons[2].pin = BUTTON_3;
   buttons[3].pin = BUTTON_4;

   for (int i = 0; i < 4; i++) {
      buttons[i].state = LOW;
      buttons[i].lastState = LOW;
      buttons[i].lastDebounceTime = 0;
      buttons[i].pushed = false;
      pinMode(buttons[i].pin, INPUT);
   }

   digitalWrite(HEAT, HIGH);
   digitalWrite(COLD, HIGH);

   pinMode(HEAT, OUTPUT);
   pinMode(COLD, OUTPUT);

   lastTempRead = millis();

   sensors.begin();

   lcd.begin(16,2);

   //lcd.home ();
   //lcd.print("Presiona una tecla");
}

void loop(){
   lcd.home ();
   printTime();
   lcd.setCursor (0, 1);
   updateButtons();
   for(int i = 0; i < 4; i++) {
      if (buttons[i].pushed) {
         lcd.print("X");
      } else {
         lcd.print("0");
      }
   }

   digitalWrite(HEAT, buttons[0].pushed ? LOW : HIGH);
   digitalWrite(COLD, buttons[1].pushed ? LOW : HIGH);

   if (millis() - lastTempRead > READ_INTERVAL) {
      sensors.requestTemperatures();
      lcd.print(" ");
      lcd.print(sensors.getTempCByIndex(0));
      lcd.print(" C");
      lastTempRead = millis();
   }

}

void updateButtons() {
   //digitalWrite(LATCH, LOW);
   //uint8_t reg = shiftIn(DATA, CLOCK, MSBFIRST);
   //Serial.println(reg);
   //reg = shiftIn(DATA, CLOCK, LSBFIRST);
   //Serial.println(reg);
   //digitalWrite(LATCH, HIGH);

   for (int i = 0; i < 4; i++) {
      int reading = digitalRead(buttons[i].pin);
      //uint8_t reading = reg & i;

      if (reading != buttons[i].lastState) {
         buttons[i].lastDebounceTime = millis();
      } 
  
      if ((millis() - buttons[i].lastDebounceTime) > DEBOUNCE_DELAY) {
         if (reading != buttons[i].state) {
            buttons[i].state = reading;
            buttons[i].pushed = reading;
         }
      }
      buttons[i].lastState = reading;
   }
}

void printTime() {
   DateTime now = RTC.now();

   lcd.print(now.day(), DEC);
   lcd.print('.');
   lcd.print(now.month(), DEC);
   lcd.print('.');
   lcd.print(now.year()-2000, DEC);
   lcd.print(' ');
   lcd.print(now.hour(), DEC);
   lcd.print(':');
   lcd.print(now.minute(), DEC);
   lcd.print(':');
   lcd.print(now.second(), DEC);
   lcd.println();
}
