#include <ESP32-TWAI-CAN.hpp>
#include <Wire.h>
#define CAN_TX GPIO_NUM_25
#define CAN_RX GPIO_NUM_26
CanFrame txFrame = { 0 };
CanFrame heartbeatFrame = { 0 };
CanFrame gyroFrame = { 0 };
uint8_t counter = 0;
uint8_t heartbeatCounter = 0;
uint8_t gyroCounter = 0;
const uint8_t MPU_ADDR = 0x68;
bool sensorFaultActive = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21,22);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Sensor Node Started");
  ESP32Can.setPins(CAN_TX, CAN_RX);
  ESP32Can.setSpeed(ESP32Can.convertSpeed(125));

  if (ESP32Can.begin()){
    Serial.println("Startup Succeeded");
  
  }else{
    Serial.println("Startup Failed");
  }
txFrame.identifier = 0x100;
txFrame.extd = 0;
txFrame.data_length_code = 7;
heartbeatFrame.identifier = 0x101;
heartbeatFrame.extd = 0;
heartbeatFrame.data_length_code = 2;
gyroFrame.identifier = 0x102;
gyroFrame.extd = 0;
gyroFrame.data_length_code = 7;
}

void loop() {
  int16_t accelX = 0;
  int16_t accelY = 0;
  int16_t accelZ = 0;
  int16_t temperature = 0;
  int16_t gyroX = 0;
  int16_t gyroY = 0;
  int16_t gyroZ = 0;

  bool sensorReadValid = false;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                 
  uint8_t i2cStatus = Wire.endTransmission(false);
  if (i2cStatus == 0){
    uint8_t bytesReceived = Wire.requestFrom(MPU_ADDR, 14);

    if (bytesReceived == 14){
      accelX = (Wire.read() << 8) | Wire.read();
      accelY = (Wire.read() << 8) | Wire.read();
      accelZ = (Wire.read() << 8) | Wire.read();
      temperature = (Wire.read() << 8) | Wire.read();
      gyroX = (Wire.read() << 8) | Wire.read();
      gyroY = (Wire.read() << 8) | Wire.read();
      gyroZ = (Wire.read() << 8) | Wire.read();
      sensorReadValid = true;
      if(sensorFaultActive){
        Serial.println("SENSOR RECOVERED\n");
        sensorFaultActive = false;
      }
    }else{
      if (!sensorFaultActive) {
    Serial.println("SENSOR FAULT ACTIVE: INCOMPLETE MPU READ\n");
    sensorFaultActive = true;
}
    }

  } else{
    if (!sensorFaultActive) {
    Serial.println("SENSOR FAULT ACTIVE: I2C COMMUNICATION");
    sensorFaultActive = true;
}
  }

heartbeatFrame.data[0] = heartbeatCounter;
if (sensorFaultActive){
  heartbeatFrame.data[1] = 0;
}else{
  heartbeatFrame.data[1] = 1;
}

  if (sensorReadValid){
    txFrame.data[0] = counter;
    txFrame.data[1] = accelX >> 8;
    txFrame.data[2] = accelX & 0xFF;
    txFrame.data[3] = accelY >> 8;
    txFrame.data[4] = accelY & 0xFF;
    txFrame.data[5] = accelZ >> 8;
    txFrame.data[6] = accelZ & 0xFF;
    gyroFrame.data[0] = gyroCounter;
    gyroFrame.data[1] = gyroX >> 8;
    gyroFrame.data[2] = gyroX & 0xFF;
    gyroFrame.data[3] = gyroY >> 8;
    gyroFrame.data[4] = gyroY & 0xFF;
    gyroFrame.data[5] = gyroZ >> 8;
    gyroFrame.data[6] = gyroZ & 0xFF;
  }


if (sensorReadValid){
    if (ESP32Can.writeFrame(txFrame)){
    Serial.println("Queued Successfully");
    counter++;
  }else{
    Serial.println("\nFAILED TO QUEUE\n");
  }
  if (ESP32Can.writeFrame(gyroFrame)) {
  Serial.println("Gyro Queued");
  gyroCounter++;
} else {
  Serial.println("\nGYRO FAILED\n");
}

}

  if(ESP32Can.writeFrame(heartbeatFrame)){
    Serial.println("Heartbeat Queued\n------------------------");
    heartbeatCounter++;
  }else{
    Serial.println("\nHEARTBEAT FAILED\n");
  }





  delay(1000);

}
