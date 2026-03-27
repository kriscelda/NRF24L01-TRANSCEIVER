#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>


#define CE_PIN    4
#define CSN_PIN   5
#define SCK_PIN   12

#define MISO_PIN  13
#define MOSI_PIN  11


#define MOTOR_ENA 16
#define MOTOR_IN1 17
#define MOTOR_IN2 18
#define LED_PIN   14
#define SERVO_PIN 15


RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"NODE1", "NODE2"};
Servo steeringServo;


struct Payload {
  int steer;
  int speed;
  int dir;
  int button;
};


Payload rxData;


void setup() {


  Serial.begin(115200);
  delay(1000);
  Serial.println("--- S3 RECEIVER BOOTING ---");


  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);


  if (!radio.begin()) {
    Serial.println("FATAL: NRF24 Hardware NOT found!");
    while (1);
  }


  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.openReadingPipe(1, address[1]);
  radio.startListening();
 
  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);   // change


  // change


  Serial.println("SUCCESS: Receiver is Ready and Listening.");
}


void loop() {


  if (radio.available()) {
    radio.read(&rxData, sizeof(rxData));
    digitalWrite(LED_PIN, HIGH);


    steeringServo.write(rxData.steer);


    if (rxData.dir == 1) { // Forward
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_ENA, rxData.speed);
    }
    else if (rxData.dir == 2) { // Backward
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
      analogWrite(MOTOR_ENA, rxData.speed);
    }
    else {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_ENA, 0);
    }
   


    Serial.printf("GOT DATA >> Steer: %d | Speed: %d | Dir: %d\n", rxData.steer, rxData.speed, rxData.dir);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
