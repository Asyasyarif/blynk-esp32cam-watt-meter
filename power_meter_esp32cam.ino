#include <PZEM004Tv30.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>


#define WIFI_SSID "PA-2.4G"
#define WIFI_PASSWORD "axios123"

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Monitoring Listrik PLN"
#define BLYNK_AUTH_TOKEN ""

#define HW_SERIAL_2_RX 14
#define HW_SERIAL_2_TX 15

#define BAUDRATE_SERIAL_1 115200
#define BAUDRATE_HW_SERIAL_2 9600

#define LED_PIN_DATA 4
#define LED_PIN_STATUS 2

#define INTERVAL_SENT_MESSAGE 1000
unsigned long prev_time = 0;

bool stateLed = false;

PZEM004Tv30 pzem(&Serial2, HW_SERIAL_2_RX, HW_SERIAL_2_TX);

void setup() {

  Serial.begin(BAUDRATE_SERIAL_1);
  Serial2.begin(BAUDRATE_HW_SERIAL_2, SERIAL_8N1, HW_SERIAL_2_RX, HW_SERIAL_2_TX);
  pinMode(LED_PIN_DATA, OUTPUT);
  pinMode(LED_PIN_STATUS, OUTPUT);
  
  digitalWrite(LED_PIN_DATA, LOW);
  digitalWrite(LED_PIN_STATUS, LOW);

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to WiFi ..");
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print('.');
  //   delay(1000);
  // }
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  Serial.println("Power Meter Ready...");
  digitalWrite(LED_PIN_STATUS, HIGH);
}

void loop() {

    if(WiFi.status() == WL_CONNECTED){
      digitalWrite(LED_PIN_STATUS, HIGH);
      Serial.print("Custom Address:");
      Serial.println(pzem.readAddress(), HEX);

      float voltage   = pzem.voltage();
      float current   = pzem.current();
      float power     = pzem.power();
      float energy    = pzem.energy();
      float frequency = pzem.frequency();
      float pf        = pzem.pf();

      if(isnan(voltage)){
          Serial.println("Error reading voltage");
      } else if (isnan(current)) {
          Serial.println("Error reading current");
      } else if (isnan(power)) {
          Serial.println("Error reading power");
      } else if (isnan(energy)) {
          Serial.println("Error reading energy");
      } else if (isnan(frequency)) {
          Serial.println("Error reading frequency");
      } else if (isnan(pf)) {
          Serial.println("Error reading power factor");
      } else {

      
        if(millis() - prev_time >= INTERVAL_SENT_MESSAGE){
          prev_time = millis();

          // Print the values to the Serial console
          Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
          Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
          Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
          Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
          Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
          Serial.print("PF: ");           Serial.println(pf);
          Serial.println();

          Blynk.virtualWrite(V0, voltage);
          Blynk.virtualWrite(V1, current);
          Blynk.virtualWrite(V2, power);
          Blynk.virtualWrite(V3, energy);
          Blynk.virtualWrite(V4, frequency);
          Blynk.virtualWrite(V5, pf);
          stateLed = !stateLed;
        }

        digitalWrite(LED_PIN_DATA, stateLed);
        Blynk.run();
      }
    }else{
      digitalWrite(LED_PIN_STATUS, LOW);
      digitalWrite(LED_PIN_DATA, LOW);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.print("Connecting to WiFi ..");
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(2000);
      }
      Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
    }
    
}
