#include "arduino_secrets.h"
#include "thingProperties.h"
#include <DHT.h>
#include <Wire.h>
#include "M5UNIT_DIGI_CLOCK.h"

#define SDA 21
#define SCL 22
#define ADD 0x30
#define DHTPIN   A0
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);
M5UNIT_DIGI_CLOCK digiClock;

unsigned long previousClockUpdate = 0;
unsigned long previousDisplayChange = 0;
unsigned long previousPlanifCheck = 0;
unsigned long previousDHTRead = 0;

// Timings
constexpr unsigned long CLOCK_INTERVAL = 9000;
constexpr unsigned long PLANIF_INTERVAL = 60000;
constexpr unsigned long DISPLAY_TIME = 3875;
constexpr unsigned long BLANK_TIME = 250;
constexpr unsigned long DHT_READ_INTERVAL = 60000;

// Temp factor correction (ex: measured temp too hot → -0.1f = -10%, too cold → 0.1f = +10%)
constexpr float DHT_TEMP_FACTOR = -0.09f;

// Mode: 0 = Time 1 = Temperature, 2 = Humidity, 3 = Blank
int displayMode = 0;
const int TOTAL_MODES = 6;
bool lastPlanifState = false;

void setup() {
  Serial.begin(9600);

  dht.begin();
  Wire.begin(SDA, SCL);
  digiClock.begin(&Wire, ADD);

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

  if (now - previousDHTRead >= DHT_READ_INTERVAL) {
    previousDHTRead = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {

      // Temp corrected
      float t_corr = t + (t * DHT_TEMP_FACTOR);

      // Temp
      temperature = round(t_corr);

      // Humidity Magnus formula
      float P_sat_measured = 6.112f * exp((17.62f * t) / (243.12f + t));
      float P_sat_corrected = 6.112f * exp((17.62f * t_corr) / (243.12f + t_corr));

      float h_corr = h * (P_sat_measured / P_sat_corrected);

      // Clamp 0 et 100 %
      if (h_corr < 0.0f) h_corr = 0.0f;
      if (h_corr > 100.0f) h_corr = 100.0f;

      // Humidity
      humidity = round(h_corr);
    }
  }

  if (now - previousClockUpdate >= CLOCK_INTERVAL) {
    previousClockUpdate = now;

    time_t rawTime = ArduinoCloud.getLocalTime();
    struct tm *timeinfo = localtime(&rawTime);
    if (timeinfo) {
      read_time = rawTime;
    }
  }

  unsigned long displayInterval = (displayMode % 2 == 0) ? DISPLAY_TIME : BLANK_TIME;
  if (now - previousDisplayChange >= displayInterval) {
    previousDisplayChange = now;
    displayMode = (displayMode + 1) % TOTAL_MODES;
    updateDisplay();
  }

  if (now - previousPlanifCheck >= PLANIF_INTERVAL) {
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
      case 0: {
        time_t rawTime = ArduinoCloud.getLocalTime();
        struct tm *timeinfo = localtime(&rawTime);
        if (timeinfo) {
          sprintf(displayStr, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        }
        break;
      }

      case 2: {
        sprintf(displayStr, " %2d°", (int)temperature);
        break;
      }

      case 4: {
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
