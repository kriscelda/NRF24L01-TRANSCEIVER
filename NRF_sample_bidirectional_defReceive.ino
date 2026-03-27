#include <Servo.h>
#include <SPI.h>
#include <RF24.h>



#define CE_PIN    4
#define CSN_PIN   5
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11

const int SERVO_PIN = 14;


RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"NODE1", "NODE2"};
Servo myServo;


struct Payload {
  int steer;
};


Payload rxData;


void setup() {
  Serial.begin(115200);

  myServo.attach(SERVO_PIN, Servo::CHANNEL_NOT_ATTACHED, 0, 180, 544, 2400, 200);


  // Explicitly tell the S3 which pins to use for SPI
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);


  if (!radio.begin()) {
    Serial.println("NRF24 Hardware NOT found!");
    while (1); // Halt if hardware is missing


    }

    Serial.println("Radio Started");

    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(76);
    radio.openReadingPipe(1, address[1]);

    radio.startListening();
    Serial.println("Receiver Ready - Awaiting Servo Data...");

    
  

}
  
void loop() {
  if (radio.available()) {
    radio.read(&rxData, sizeof(rxData));
   
    // Constraints check to keep servo safe
    if(rxData.steer >= 0 && rxData.steer <= 180) {
      myServo.write(rxData.steer);
      Serial.print("Moving Servo to: ");
      Serial.println(rxData.steer);
    }
  }
}
