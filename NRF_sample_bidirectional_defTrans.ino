#include <SPI.h>
#include <RF24.h>

#define CE_PIN    4
#define CSN_PIN   5
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11
#define POT_PIN   1


RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"NODE1", "NODE2"};


struct Payload {
  int steer; // Only sending steering data
};


Payload txData;

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);

  if (!radio.begin()) {
    Serial.println("NRF ERROR: Check wiring!");
    while (1);
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);

  radio.openWritingPipe(address[1]);

  radio.stopListening();
  Serial.println("Transmitter Ready - Servo Test Mode");
}


void loop() {
  int potVal = analogRead(POT_PIN);
  // Map 0-4095 (ESP32 ADC) to 0-180 (Servo Degrees)
  txData.steer = map(potVal, 0, 4095, 0, 180);

  bool ok = radio.write(&txData, sizeof(txData));

  if (ok) {
    Serial.print("Sent Steer Angle: ");
    Serial.println(txData.steer);
  } else {
    Serial.println("Send Failed: Receiver not active?");
  }

  delay(1000); // Fast enough for smooth movement
}





