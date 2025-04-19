#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <string>
#include <FastLED.h>
#include <WiFi.h>

#define LED_PIN     21  // WS2812 LED on GPIO21
#define NUM_LEDS    1   // We have one RGB LED
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

BLEAdvertising* pAdvertising;

constexpr int cycleDurationMs = 120000;
constexpr int stepIntervalMs  = 320;
constexpr int stepsPerCycle   = cycleDurationMs / stepIntervalMs;

int currentStep = 0;

void setLED(CRGB color) {
  leds[0] = color;
  FastLED.show();
  FastLED.delay(1);  // Ensure the color is set
}

void setup()
{
  setCpuFrequencyMhz(80);
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { }

  WiFi.mode(WIFI_OFF);
  btStop();
  btStart(); 

  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(20);  
  FastLED.clear();  // Clear any existing colors
  FastLED.show();
  
  // Show blue color during boot
  setLED(CRGB(0, 0, 255));  // Pure blue
  
  Serial.println("Booting…");

  BLEDevice::init("M3");
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(false);
  pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
  
  // Add device name to advertisement data
  BLEAdvertisementData advData;
  advData.setName("M3");
  pAdvertising->setAdvertisementData(advData);
  
  pAdvertising->start();

  Serial.println("M3 bike BLE advertising started…");
  
  // Show green when ready
  setLED(CRGB(0, 255, 0));  // Pure green
}

void updateAdData()
{
  // Flash white briefly when sending data
  setLED(CRGB(255, 255, 255));  // Pure white
  
  const int  halfCycle = stepsPerCycle / 2;
  const bool rampUp    = currentStep < halfCycle;
  const float progress = (currentStep % halfCycle) / static_cast<float>(halfCycle);

  const uint16_t power = rampUp
                         ? (30  + progress * (300 - 30))
                         : (300 - progress * (300 - 30));
  const uint8_t rpm    = rampUp
                         ? (60  + progress * (90  - 60))
                         : (90  - progress * (90  - 60));
  const uint8_t hr     = rampUp
                         ? (90  + progress * (160 - 90))
                         : (160 - progress * (160 - 90));

  static float distance = 0.0f;
  distance += rpm / 3600.0f * (stepIntervalMs / 1000.0f);

  uint8_t payload[19] = {0};  // Total manufacturer data size (19 bytes)
  // Manufacturer Specific Data
  payload[0] = 0x02;  // Manufacturer ID (0x0102) - little endian
  payload[1] = 0x01;
  payload[2] = 0x06;  // Version major
  payload[3] = 0x30;  // Version minor
  payload[4] = 0x00;  // Data type
  payload[5] = 0x01;  // Equipment ID
  payload[6] = (rpm * 10) & 0xFF;     // Cadence (little endian, multiplied by 10)
  payload[7] = (rpm * 10) >> 8;
  payload[8] = (hr * 10) & 0xFF;      // Heart rate (little endian, multiplied by 10)
  payload[9] = (hr * 10) >> 8;
  payload[10] = power & 0xFF;  // Power (little endian)
  payload[11] = power >> 8;
  payload[12] = 0x0D;         // Calories (little endian)
  payload[13] = 0x00;
  payload[14] = 0x04;         // Duration Minutes
  payload[15] = 0x27;         // Duration Seconds
  payload[16] = 0x01;         // Distance (little endian)
  payload[17] = 0x00;
  payload[18] = 0x0A;         // Gear

  BLEAdvertisementData advData;
  advData.setFlags(0x04);  // LE General Discoverable Mode
  advData.setName("M3");
  advData.setManufacturerData(std::string(reinterpret_cast<char*>(payload), sizeof(payload)));
  pAdvertising->setAdvertisementData(advData);

  // Return to green after sending
  setLED(CRGB(0, 255, 0));  // Pure green
  
  Serial.printf("Step %3d | RPM=%3u | Power=%3u | HR=%3u | Dist=%.2f km\n",
                currentStep, rpm, power, hr, distance);

  currentStep = (currentStep + 1) % stepsPerCycle;
}

void loop()
{
  updateAdData();
  delay(stepIntervalMs);
}