/*
* Name: m3xBLE
* Purpose: To allow control of the M3X arm robot via bluetooth
* Author: Jacob Breen
* Date Started: 3/8/23
* Date Last Worked on: 3/15/23
*
* Version: 00.00.01
*/

/*
X(e54aa130-bdbf-11ed-a901-0800200c9a66)
X(e54aa131-bdbf-11ed-a901-0800200c9a66)
X(e54aa132-bdbf-11ed-a901-0800200c9a66)
e54aa133-bdbf-11ed-a901-0800200c9a66
*/

#if ( defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || \
      defined(ARDUINO_GENERIC_RP2040) ) && !defined(ARDUINO_ARCH_MBED)
  #if !defined(RP2040_ISR_SERVO_USING_MBED)    
    #define RP2040_ISR_SERVO_USING_MBED     false
  #endif  
  
#elif ( defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || \
      defined(ARDUINO_GENERIC_RP2040) ) && defined(ARDUINO_ARCH_MBED)
      
  #if !defined(RP2040_ISR_SERVO_USING_MBED)    
    #define RP2040_ISR_SERVO_USING_MBED     true
  #endif  
  
#else      
  #error This code is intended to run on the mbed / non-mbed RP2040 platform! Please check your Tools->Board setting.
#endif

#define TIMER_INTERRUPT_DEBUG       0
#define ISR_SERVO_DEBUG             0

#include <ArduinoBLE.h>
#include <String.h>
#include "RP2040_ISR_Servo.h"

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS        800
#define MAX_MICROS        2450 

#define SERVO_PIN_1       12

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

#define NUM_SERVOS            1

ISR_servo_t ISR_servo = { -1, SERVO_PIN_1 };

BLEService MessageService("e54aa130-bdbf-11ed-a901-0800200c9a66");

BLEStringCharacteristic commandData("e54aa131-bdbf-11ed-a901-0800200c9a66", BLERead | BLEWrite | BLENotify, 16);
BLEStringCharacteristic responseData("e54aa132-bdbf-11ed-a901-0800200c9a66", BLEWrite | BLENotify, 16);

bool status = false;

void setup() {
  pinMode(ISR_servo.servoPin, OUTPUT);
  digitalWrite(ISR_servo.servoPin, LOW);
  int position = 0;      // position in degrees, initialize to zero

  
  Serial.begin(57600);
  if(!BLE.begin()){
    Serial.println("BLE Failed");
  }

  delay(200);

  #if defined(ARDUINO_ARCH_MBED)
  //Serial.println(F("\nStarting RP2040_MultipleRandomServos on Mbed "));
  #else
  //Serial.println(F("\nStarting RP2040_MultipleRandomServos on "));
  #endif

  //Serial.println(BOARD_NAME);
  //Serial.println(RP2040_ISR_SERVO_VERSION);

    ISR_servo.servoIndex = RP2040_ISR_Servos.setupServo(ISR_servo.servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo.servoIndex != -1)
    {
     //Serial.println(F("Setup OK Servo index = ")); //Serial.println(ISR_servo.servoIndex);
      RP2040_ISR_Servos.setPosition(ISR_servo.servoIndex, 0);
    }
    else
    {
     //Serial.println(F("Setup Failed Servo index = ")); //Serial.println(ISR_servo.servoIndex);
    }

  BLE.setDeviceName("m3xBLE");
  BLE.setLocalName("m3xBLE");
  commandData.writeValue(" ");
  MessageService.addCharacteristic(commandData);
  MessageService.addCharacteristic(responseData);

  BLE.addService(MessageService);
  BLE.advertise();
  Serial.println("\nNano is waiting for connections...");
}

void hapticStimulus(int position)
{
  position = position;
  //Serial.println(F("Haptic stimulus initiated"));
  RP2040_ISR_Servos.setPosition(ISR_servo.servoIndex, position );

  // hold the stimiulus for 1 second
  delay(1000);

  position = 0;
  //Serial.println(F("Servo back to home, 0 degree"));
  RP2040_ISR_Servos.setPosition(ISR_servo.servoIndex, position );
}

void loop() {
  BLEDevice central = BLE.central();

  if(central && central.connected()) {
    //Serial.print("Connected to device: ");
    //Serial.println(central.address());
    //if(commandData.valueUpdated()) {

      //Serial.println("Value Received");
      //byte byteData[16];
      //commandData.readValue(byteData, 16);
      //message = String((char *)byteData);
    if(commandData.written()) {
      String message = commandData.value();
      commandData.writeValue(" ");

      if(message == "ON") {
        Serial.println("ON");
        hapticStimulus(90);
        status = true;
        //commandData.writeValue("STARTING");
      } else if(message == "OFF") {
        Serial.println("OFF");
        status = false;
        //commandData.writeValue("STOPPING");
      } else if(message == "STATUS") {
        Serial.print("STATUS: ");
        String Data;
        if(status) {
          Serial.println("ACTIVE");
          Data = "ACTIVE";
        } else {
          Serial.println("NOT ACTIVE");
          Data = "NOT ACTIVE";
        }
        //responseData.writeValue(Data);
      } else if(message == " ") {
      } else {
        Serial.print("INVALID WRITE: ");
        Serial.println(message);
        //commandData.writeValue("NON VALID");
      }
    }

  } else if(!central.connected()) {
    //Serial.print("Disconnected from Device ");
    //Serial.println(central.address());
  }

}
