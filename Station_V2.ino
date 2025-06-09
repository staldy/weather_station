#include "arduino_secrets.h"
#include "thingProperties.h"          
#include <DHT.h>
#include <Wire.h>
#include "M5UNIT_DIGI_CLOCK.h"

// --- Constantes ---
#define SDA 21
#define SCL 22
#define ADD 0x30
#define DHTPIN   A0
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);
M5UNIT_DIGI_CLOCK digiClock;

unsigned long previousClockUpdate = 0;
unsigned long previousDHTread = 0;
unsigned long previousDisplayChange = 0;
unsigned long previousPlanifCheck = 0;
constexpr unsigned long CLOCK_INTERVAL = 1000;  
constexpr unsigned long DHT_INTERVAL = 6000;  
constexpr unsigned long DISPLAY_TIME = 2875;    
constexpr unsigned long BLANK_TIME = 250;   

// Mode: 0 = Time 1 = Temperature, 2 = Humidity, 3 = Blank
int displayMode = 0;
const int TOTAL_MODES = 6;  // 3 Modes + 3 Blanks
bool lastPlanifState = false;

void setup() {
  Serial.begin(9600);

  dht.begin();
  Wire.begin(SDA, SCL);
digiClock.begin(&Wire, ADD);
  
  // Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  digiClock.setBrightness(brightness);
  
  lastPlanifState = planif.isActive();
}

void loop() {
  ArduinoCloud.update();

  unsigned long now = millis();

  if (now - previousClockUpdate >= CLOCK_INTERVAL) {
    previousClockUpdate = now;

    time_t rawTime = ArduinoCloud.getLocalTime();
    struct tm *timeinfo = localtime(&rawTime);
    if (timeinfo) {
      read_time = rawTime;
    }
  }
  
  if (now - previousDHTread >= DHT_INTERVAL) {
    previousDHTread = now;
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (!isnan(h) && !isnan(t)) {
     humidity = round(h), 0;
     temperature = round(t * 10.0) / 10.0;      
     
    }
  }
  
  // Mode
   unsigned long displayInterval = (displayMode % 2 == 0) ? DISPLAY_TIME : BLANK_TIME;
  if (now - previousDisplayChange >= displayInterval) {
       previousDisplayChange = now;
    // Next
    displayMode = (displayMode + 1) % TOTAL_MODES;
     updateDisplay();
  }
    // Check
 if (now - previousPlanifCheck >= 1000) { 
    previousPlanifCheck = now;
    
    bool currentPlanifState = planif.isActive();
    if (currentPlanifState != lastPlanifState) {
      lastPlanifState = currentPlanifState;
      
       digiClock.setBrightness(currentPlanifState ? 0 : brightness);
   }
 }
 }

void updateDisplay() {
  char displayStr[6] = {0}; 
  
  if (displayMode % 2 != 0) {
  
    strcpy(displayStr, "    ");
  } else {
  
    switch (displayMode) {
      case 0: // Time
      {
        time_t rawTime = ArduinoCloud.getLocalTime();
        struct tm *timeinfo = localtime(&rawTime);
        if (timeinfo) {
          
          sprintf(displayStr, "%02d:%02d", 
                 timeinfo->tm_hour,
                 timeinfo->tm_min);
        }
        break;
      }
      
   case 2: // Temperature
      {
        char tempStr[6];
        dtostrf(temperature, 4, 1, tempStr);
        sprintf(displayStr, "%s", tempStr);
        break;
      }
      

      case 4: // Humidity
      {
        sprintf(displayStr, " H:%2d", (int)humidity);
        
        break;
      }
    }
  }
  
  digiClock.setString(displayStr);

   if (displayMode == 2) {
digiClock.setSegments(4, 0b01100011); 
     
}
}


void onBrightnessChange() {
  if (!planif.isActive()) {
  digiClock.setBrightness(brightness);
}
}

void onPlanifChange() {
 digiClock.setBrightness(planif.isActive() ? 0 : brightness);
}

