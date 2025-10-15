#include <Arduino.h>
#include <WiFi.h>
#include <ModbusMaster.h>

const char* ssid = "M15";
const char* password = "1234567890";

#define RX_PIN 26
#define TX_PIN 27
#define RE_DE_PIN 25

ModbusMaster node;

/* ======= RS485 Transmission Control ======= */
void preTransmission() {
  digitalWrite(RE_DE_PIN, HIGH);
  delayMicroseconds(10);
}

void postTransmission() {
  digitalWrite(RE_DE_PIN, LOW);
  delayMicroseconds(10);
}

/* ======= WiFi Setup ======= */
void setup_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}


/* ======= Read the NPK Sensor (7 registers) ======= */
void read_npk_sensor() {
  // Read registers starting at 0x0000 (7 registers)
  uint8_t result = node.readHoldingRegisters(0x0000, 7);
  if (result == node.ku8MBSuccess) {

    Serial.println("Raw Modbus Data (Registers 0-6):");
    for (int i = 0; i < 7; i++) {
      uint16_t raw_value = node.getResponseBuffer(i);
      Serial.printf("  Register[%d]: %d\n", i, raw_value);
    }

    float humidity     = node.getResponseBuffer(0) * 0.1;
    float temperature  = (int16_t)node.getResponseBuffer(1) * 0.1;
    float conductivity = node.getResponseBuffer(2);
    float ph           = node.getResponseBuffer(3) * 0.1;
    float nitrogen     = node.getResponseBuffer(4);
    float phosphorus   = node.getResponseBuffer(5);
    float potassium    = node.getResponseBuffer(6);
    
    Serial.printf("NPK Sensor - Humidity: %.1f %%RH\n", humidity);
    Serial.printf("NPK Sensor - Temperature: %.1f °C\n", temperature);
    Serial.printf("NPK Sensor - Conductivity: %.1f uS/cm\n", conductivity);
    Serial.printf("NPK Sensor - pH: %.1f\n", ph);
    Serial.printf("NPK Sensor - Nitrogen: %.0f mg/kg\n", nitrogen);
    Serial.printf("NPK Sensor - Phosphorus: %.0f mg/kg\n", phosphorus);
    Serial.printf("NPK Sensor - Potassium: %.0f mg/kg\n", potassium);
    Serial.println("------------------------");
  } else {
    Serial.printf("NPK Sensor - Modbus Error: 0x%X\n", result);
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(RE_DE_PIN, OUTPUT);
  digitalWrite(RE_DE_PIN, LOW);
  // pinMode(BUZZER_PIN, OUTPUT);
  // digitalWrite(BUZZER_PIN, LOW);

  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  setup_wifi();
}

void loop() {
  read_npk_sensor();
  delay(1000);
}