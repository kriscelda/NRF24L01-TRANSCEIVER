#include <SPI.h>
#include <RF24.h>


// --- NRF24 PINS (Stable for ESP32-S3) ---
#define CE_PIN    4
#define CSN_PIN   5
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11


// --- INPUT PINS ---
#define POT_PIN    1  
#define JOY_Y_PIN  2  
#define JOY_SW     3  
#define LED_PIN    14


RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"NODE1", "NODE2"};


struct Payload {
  int steer;
  int speed;
  int dir;
  int button;
};


Payload txData;


void setup() {
  Serial.begin(115200);
  delay(1000); // Give Serial time to start
  Serial.println("\n--- S3-WROOM TRANSMITTER STARTING ---");


  pinMode(LED_PIN, OUTPUT);
  pinMode(JOY_SW, INPUT_PULLUP);


  // Force SPI for S3 hardware
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);


  if (!radio.begin()) {
    Serial.println("NRF ERROR: Hardware not found! Check your wiring.");
    while (1);
  }


  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15, 15); // Maximize chances of communication
 
  // Use address[1] to talk to the Receiver
  radio.openWritingPipe(address[1]);
  radio.stopListening();
 
  Serial.println("TRANSMITTER READY: Sending Control Data...");
}


void loop() {
  // 1. Read Physical Inputs
  int potVal = analogRead(POT_PIN);
  int joyY = analogRead(JOY_Y_PIN);
  txData.button = digitalRead(JOY_SW);


  // 2. Map Steering (0-90 Left, 90 Center, 90-180 Right)
  txData.steer = map(potVal, 0, 4095, 0, 180);


  // 3. Throttle Logic
  if (joyY > 2200) {
    txData.speed = map(joyY, 2200, 4095, 0, 255);
    txData.dir = 1; // Forward
  } else if (joyY < 1800) {
    txData.speed = map(joyY, 1800, 0, 0, 255);
    txData.dir = 2; // Backward
  } else {
    txData.speed = 0;
    txData.dir = 0; // Idle
  }


  // 4. Send the Data
  bool ok = radio.write(&txData, sizeof(txData));
  digitalWrite(LED_PIN, ok ? HIGH : LOW);


  // 5. YOUR REQUESTED DEBUGGING FORMAT
  if (ok) Serial.print("SENT SUCCESS | ");
  else Serial.print("SEND FAILED  | ");


  // Steering Debug
  if (txData.steer < 80) Serial.print("Turning LEFT | ");
  else if (txData.steer > 100) Serial.print("Turning RIGHT | ");
  else Serial.print("Straight | ");


  // Throttle Debug
  if (txData.dir == 1) Serial.print("Moving FORWARD (PWM: ");
  else if (txData.dir == 2) Serial.print("Moving BACKWARD (PWM: ");
  else Serial.print("IDLE (PWM: ");
  Serial.print(txData.speed);
  Serial.print(") | ");


  // Button Debug
  if (txData.button == LOW) Serial.println("SW: PRESSED");
  else Serial.println("SW: RELEASED");


  delay(40);
}
